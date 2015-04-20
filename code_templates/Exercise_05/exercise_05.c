#include <stdio.h>
#include "platform.h"
#include "xgpio.h"
#include "xparameters.h"


int main()
{
	XGpio_Config *GPIO_Config;
	XGpio my_Gpio;

	// Declare some variables that we will use later
	int Status;
	unsigned int DIP_value;
	unsigned int LED_value;


	init_platform();

	printf("Exercise 5\n\r");

	// Lookup the config information and store it in the struct "GPIO_Config"
	<ADD SOME CODE IN HERE>

	// Initialise the GPIO using a reference to the my_Gpio struct,
	// the struct "GPIO_Config", and also a base address value.
	Status = <ADD SOME CODE IN HERE>

	// Set the direction of the bits in the GPIO.
	// The lower (LSB) 8 bits of the GPIO are for the DIP Switches (inputs).
	// The upper (MSB) 8 bits of the GPIO are for the LEDs (outputs).
	<ADD SOME CODE IN HERE>


	// Go around in a loop for ever
	while (1)
	{
		// Read from the GPIO to determine the position of the DIP switches
		<ADD SOME CODE IN HERE>

		// Assign a value to LED_Value variable, adjusting it as necessary
		<ADD SOME CODE IN HERE>

		// Print the values of the variables to the UART to help us debug
		xil_printf("DIP = 0x%04X, LED = 0x%04X\n\r", DIP_value, LED_value);

		// Write the value back to the GPIO
		<ADD SOME CODE IN HERE>
	}

	// Technically we should never reach this far!
	return (0);
}
