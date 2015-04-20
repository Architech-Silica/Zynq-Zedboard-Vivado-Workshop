#include <stdio.h>
#include "platform.h"
#include "xgpiops.h"
#include "xparameters.h"


int main()
{
	XGpioPs_Config *GPIO_Config;
	XGpioPs my_Gpio;
	int Status;

	init_platform();

	printf("Exercise 02\n\r");

	GPIO_Config = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
	Status = XGpioPs_CfgInitialize(&my_Gpio, GPIO_Config, GPIO_Config->BaseAddr);

	XGpioPs_SetDirectionPin(&my_Gpio, 7, 1);

	while(1)
	{
		XGpioPs_WritePin(&my_Gpio, 7, 0);
		for (Status=0; Status< 2000; Status++)
		{
			print(".");
		}
		XGpioPs_WritePin(&my_Gpio, 7, 1);
		for (Status=0; Status< 2000; Status++)
		{
			print(".");
		}
	}

	return 0;
}
