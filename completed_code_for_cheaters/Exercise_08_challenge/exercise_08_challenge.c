#include <stdio.h>
#include "platform.h"
#include "xscutimer.h"
#include "xparameters.h"
#include "xscugic.h"
#include "xgpio.h"
#include "xgpiops.h"


#define INTERRUPT_COUNT_TIMEOUT_VALUE 50

// Function prototypes
static void my_timer_interrupt_handler(void *CallBackRef);


// Global variables
int InterruptCounter = 0;
int Flashing_LED_state = 0;



int main()
{
	init_platform();

	// Declare variables that we'll use later
	int Status;
	unsigned int DIP_value;
	unsigned int LED_value;

	// Declare two structs.  One for the Timer instance, and
	// the other for the timer's config information
	XScuTimer my_Timer;
	XScuTimer_Config *Timer_Config;

	// Declare two structs.  One for the General Interrupt
	// Controller (GIC) instance, and the other for config information
	XScuGic my_Gic;
	XScuGic_Config *Gic_Config;

	// Declarations of the GPIO structs
	XGpio_Config *GPIO_Config;
	XGpio my_Gpio;
	XGpioPs_Config *PS_GPIO_Config;
	XGpioPs my_PS_Gpio;

	// Lookup the config information and store it in the struct "GPIO_Config"
	GPIO_Config = XGpio_LookupConfig(XPAR_AXI_GPIO_0_DEVICE_ID);
	PS_GPIO_Config = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);

	// Initialise the GPIO using a reference to the my_Gpio struct,
	// the struct "GPIO_Config", and also a base address value.
	Status = XGpio_CfgInitialize(&my_Gpio, GPIO_Config, GPIO_Config->BaseAddress);
	Status = XGpioPs_CfgInitialize(&my_PS_Gpio, PS_GPIO_Config, PS_GPIO_Config->BaseAddr);

	// Set the direction of the bits in the GPIO.
	// The lower (LSB) 8 bits of the GPIO are for the DIP Switches (inputs).
	// The upper (MSB) 8 bits of the GPIO are for the LEDs (outputs).
	XGpio_SetDataDirection(&my_Gpio, 1, 0x00FF);
	XGpioPs_SetDirectionPin(&my_PS_Gpio,7,1);

	// Look up the config information for the GIC
	Gic_Config = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);

	// Initialise the GIC using the config information
	Status = XScuGic_CfgInitialize(&my_Gic, Gic_Config, Gic_Config->CpuBaseAddress);

	// Look up the the config information for the timer
	Timer_Config = XScuTimer_LookupConfig(XPAR_PS7_SCUTIMER_0_DEVICE_ID);

	// Initialise the timer using the config information
	Status = XScuTimer_CfgInitialize(&my_Timer, Timer_Config, Timer_Config->BaseAddr);

	// Initialize Exception handling on the ARM processor
	Xil_ExceptionInit();

	// Connect the supplied Xilinx general interrupt handler
	// to the interrupt handling logic in the processor.
	// All interrupts go through the interrupt controller, so the
	// ARM processor has to first "ask" the interrupt controller
	// which peripheral generated the interrupt.  The handler that
	// does this is supplied by Xilinx and is called "XScuGic_InterruptHandler"
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, &my_Gic);

	// Assign (connect) our interrupt handler for our Timer
	Status = XScuGic_Connect(&my_Gic, XPAR_SCUTIMER_INTR, (Xil_ExceptionHandler)my_timer_interrupt_handler, (void *)&my_Timer);

	// Enable the interrupt *input* on the GIC for the timer's interrupt
	XScuGic_Enable(&my_Gic, XPAR_SCUTIMER_INTR);

	// Enable the interrupt *output* in the timer.
	XScuTimer_EnableInterrupt(&my_Timer);

	// Enable interrupts in the ARM Processor.
	Xil_ExceptionEnable();


	// Load the timer with a value that represents one second of real time
	// HINT: The SCU Timer is clocked at half the frequency of the CPU.
	XScuTimer_LoadTimer(&my_Timer, XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2);

	// Enable Auto reload mode on the timer.  When it expires, it re-loads
	// the original value automatically.  This means that the timing interval
	// is never skewed by the time taken for the interrupt handler to run
	XScuTimer_EnableAutoReload(&my_Timer);

	// Start the SCU timer running (it counts down)
	XScuTimer_Start(&my_Timer);


	// Create an infinite loop of nothing-ness
	while(1)
	{
		// Read from the GPIO to determine the position of the DIP switches
		DIP_value = XGpio_DiscreteRead(&my_Gpio, 1);

		// Mask the upper 8 bits, so that the value from the previous iteration is not re-read.
		DIP_value = DIP_value & 0x00FF;

		// Assign a value to LED_Value variable, adjusting it as necessary
		LED_value = DIP_value << 8;

		// Write the current status of the flashing LED to the GPIO
		XGpioPs_WritePin(&my_PS_Gpio,7,Flashing_LED_state);

		// Print the values of the variables to the UART to help us debug
		//printf("DIP = 0x%04X, LED = 0x%04X\n\r", DIP_value, LED_value);

		// Write the value back to the GPIO
		XGpio_DiscreteWrite(&my_Gpio, 1, LED_value);

		// Check to see if we've serviced more than 20 interrupts
		if (InterruptCounter >= INTERRUPT_COUNT_TIMEOUT_VALUE)
		{
			// Break out of the while loop
			break;
		}
	}

	// Print a message to the UART to show that we've made it out of the while loop
	printf("If we see this message, then we've broken out of the while loop\n\r");


	// Disable interrupts in the Processor.
	Xil_ExceptionDisable();

	// Disconnect the interrupt for the Timer.
	XScuGic_Disconnect(&my_Gic, XPAR_SCUTIMER_INTR);

	cleanup_platform();

	return 0;
}


static void my_timer_interrupt_handler(void *CallBackRef)
{
	// The Xilinx drivers automatically pass an instance of
	// the peripheral which generated in the interrupt into this
	// function, using the special parameter called "CallBackRef".
	// We will locally declare an instance of the timer, and assign
	// it to CallBackRef.  You'll see why in a minute.
	XScuTimer *my_Timer_LOCAL = (XScuTimer *) CallBackRef;

	// Here we'll check to see if the timer counter has expired.
	// Technically speaking, this check is not necessary.
	// We know that the timer has expired because that's the
	// reason we're in this handler in the first place!
	// However, this is an example of how a callback reference
	// can be used as a pointer to the instance of the timer
	// that expired.  If we had two timers then we could use the same
	// handler for both, and the "CallBackRef" would always tell us
	// which timer had generated the interrupt.
	if (XScuTimer_IsExpired(my_Timer_LOCAL))
	{
		// Clear the interrupt flag in the timer, so we don't service
		// the same interrupt twice.
		XScuTimer_ClearInterruptStatus(my_Timer_LOCAL);

		// Increment a counter so that we know how many interrupts
		// have been generated.  The counter is a global variable
		InterruptCounter++;

		// Update the value of the variable that stores the flashing LED state
		if (Flashing_LED_state > 0)
		{
			Flashing_LED_state = 0;
		}
		else
		{
			Flashing_LED_state++;
		}

		// Print something to the UART to show that we're in the interrupt handler
		printf("\n\r** This message comes from the interrupt handler! (%d) **\n\r\n\n\r", InterruptCounter);

		// Check to see if we've had more than the defined number of interrupts
		if (InterruptCounter >= INTERRUPT_COUNT_TIMEOUT_VALUE)
		{
			// Stop the timer from automatically re-loading, so
			// that we don't get any more interrupts.
			// We couldn't do this if we didn't have the CallBackRef
			XScuTimer_DisableAutoReload(my_Timer_LOCAL);
		}
	}
}


