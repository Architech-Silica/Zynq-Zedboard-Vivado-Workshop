#include <stdio.h>
#include "platform.h"
#include "xscutimer.h"
#include "xparameters.h"


int main()
{
	init_platform();

	// Declare some variables that we'll use later
	int Status;
	int timer_value;
	int counter = 0;

	// Declare two structs.  One for the Timer instance, and
	// the other for the timer's config information
	XScuTimer my_Timer;
	XScuTimer_Config *Timer_Config;

	// Look up the the config information for the timer
	Timer_Config = XScuTimer_LookupConfig(XPAR_PS7_SCUTIMER_0_DEVICE_ID);

	// Initialise the timer using the config information
	Status = XScuTimer_CfgInitialize(&my_Timer, Timer_Config, Timer_Config->BaseAddr);

	// Load the timer with a value that represents one second
	// The SCU Timer is clocked at half the freq of the CPU.
	XScuTimer_LoadTimer(&my_Timer, XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2);

	// Start the timer running (it counts down)
	XScuTimer_Start(&my_Timer);

	// An infinite loop
	while(1)
	{
		// Read the value of the timer
		timer_value = XScuTimer_GetCounterValue(&my_Timer);

		// If the timer has reached zero
		if (timer_value == 0)
		{
			// Re-load the original value into the timer and re-start it
			XScuTimer_RestartTimer(&my_Timer);

			// Write something to the UART (and count the seconds)
			printf("Timer has reached zero %d times\n\r", counter++);
		}
		else
		{
			// Show the value of the timer's counter value, for debugging purposes
			//printf("Timer is still running (Timer value = %d)\n\r", timer_value);
		}
	}

	cleanup_platform();

	return 0;
}
