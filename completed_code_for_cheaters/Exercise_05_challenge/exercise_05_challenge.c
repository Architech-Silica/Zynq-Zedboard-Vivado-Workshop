#include "platform.h"
#include "xgpio.h"
#include "xgpiops.h"
#include "xparameters.h"
#include <stdio.h>


int main()
{
	// Declarations of the config struct
	XGpio_Config *GPIO_Config;
	XGpioPs_Config *PS_GPIO_Config;

	// Declarations of the GPIO instance struct
	XGpio my_Gpio;
	XGpioPs my_PS_Gpio;

	// Declare some variables that we will use later
	int Status;
	unsigned int DIP_value;
	unsigned int LED_value;

	int switch_count;
	int test_mask;


	init_platform();

	printf("Exercise 5\n\r");

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


	// Go around in a loop for ever
	while (1)
	{
		// Read from the GPIO to determine the position of the DIP switches
		DIP_value = XGpio_DiscreteRead(&my_Gpio, 1);

		// Mask the upper 8 bits, so that the value from the previous iteration is not re-read.
		DIP_value = DIP_value & 0x00FF;

		// Assign a value to LED_Value variable, adjusting it as necessary
		LED_value = DIP_value << 8;

		// Print the values of the variables to the UART to help us debug
		printf("DIP = 0x%04X, LED = 0x%04X\n\r", DIP_value, LED_value);

		// Write the value back to the GPIO
		XGpio_DiscreteWrite(&my_Gpio, 1, LED_value);

		switch_count = 0;
		for(test_mask=1; test_mask<=0x80; test_mask*=2)
		{
			if (DIP_value & test_mask)
			{
				switch_count++;
			}
		}
		if (switch_count >=4)
		{
			XGpioPs_WritePin(&my_PS_Gpio,7,1);
		}
		else
		{
			XGpioPs_WritePin(&my_PS_Gpio,7,0);
		}
	}

	// Technically we should never reach this far!
	return (0);
}

