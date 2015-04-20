/***************************** Include Files *********************************/

#include "xparameters.h"
#include "xscugic.h"
#include "xil_exception.h"
#include <stdio.h>
#include "xspi.h"
#include "math.h"

#define MAX31723_CONFIG_REG_ADDRESS 0x80
#define MAX31723_TEMP_LSB_REG_ADDRESS 0x01
#define MAX31723_TEMP_MSB_REG_ADDRESS 0x02

#define MAX31723_THERMOMETER_RESOLUTION_9BIT_MODE 0x00
#define MAX31723_THERMOMETER_RESOLUTION_10BIT_MODE 0x02
#define MAX31723_THERMOMETER_RESOLUTION_11BIT_MODE 0x04
#define MAX31723_THERMOMETER_RESOLUTION_12BIT_MODE 0x06

#define MAX31723_CONTINUOUS_TEMPERATURE_CONVERSION_MODE 0x00
#define MAX31723_COMPARATOR_MODE 0x00
#define MAX31723_DISABLE_ONE_SHOT_TEMPERATURE_CONVERSION 0x0


// Define the temperature calibration offset here
#define TEMPERATURE_CALIBRATION_OFFSET -3.30

/*
 *  This is the size of the buffer to be transmitted/received in this example.
 */
#define BUFFER_SIZE 4


/**************************** Type Definitions *******************************/
/*
 * The following data type is used to send and receive data on the SPI
 * interface.
 */
typedef u8 DataBuffer[BUFFER_SIZE];

/************************** Function Prototypes ******************************/

void SpiIntrHandler(void *CallBackRef, u32 StatusEvent, u32 ByteCount);
void display_buffers(void);
void clear_SPI_buffers(void);
float read_current_temperature(XSpi *SpiInstance);


/************************** Variable Definitions *****************************/


/*
 * The following variables are shared between non-interrupt processing and
 * interrupt processing such that they must be global.
 */
volatile int SPI_TransferInProgress;
int SPI_Error_Count;


/*
 * The following variables are used to read and write to the  Spi device, they
 * are global to avoid having large buffers on the stack.
 */
u8 ReadBuffer[BUFFER_SIZE];
u8 WriteBuffer[BUFFER_SIZE];



int main(void)
{
	XSpi_Config *SPI_ConfigPtr;
	XScuGic_Config *IntcConfig;
	XScuGic IntcInstance;		/* Interrupt Controller Instance */
	static XSpi SpiInstance;	 /* The instance of the SPI device */

	int Status;
	float temperature;
	float previous_temperature;


	// Initialise the SPI driver so that it is ready to use.
	<Add several lines of code here to set up the drivers for the SPI peripheral>

	// Reset the SPI peripheral
	XSpi_Reset(&SpiInstance);

	// Initialise the Interrupt controller so that it is ready to use.
	IntcConfig = XScuGic_LookupConfig(XPAR_SCUGIC_0_DEVICE_ID);
	if (NULL == IntcConfig) return XST_FAILURE;
	Status = XScuGic_CfgInitialize(&IntcInstance, IntcConfig, IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) return XST_FAILURE;

	// Initialise exceptions on the ARM processor
	Xil_ExceptionInit();

	// Connect the interrupt controller interrupt handler to the hardware interrupt handling logic in the processor.
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, &IntcInstance);


	// Connect a device driver handler that will be called when an interrupt
	// for the device occurs, the device driver handler performs the
	// specific interrupt processing for the device.
	Status = XScuGic_Connect(&IntcInstance, XPAR_FABRIC_AXI_QUAD_SPI_0_IP2INTC_IRPT_INTR, (Xil_ExceptionHandler)XSpi_InterruptHandler, (void *)&SpiInstance);
	if (Status != XST_SUCCESS) return Status;


	// Enable the interrupt for the SPI peripheral.
	<Enable the interrupt input for the SPI peripheral on the interrupt controller

	// Enable interrupts in the Processor.
	Xil_ExceptionEnable();


	// Perform a self-test to ensure that the hardware was built correctly.
	Status = XSpi_SelfTest(&SpiInstance);
	if (Status != XST_SUCCESS) return XST_FAILURE;

	printf("MAX31723PMB1 PMOD test\n\r\n\r");

	// Run loopback test only in case of standard SPI mode.
	if (SpiInstance.SpiMode != XSP_STANDARD_MODE) return XST_SUCCESS;

	// Setup the handler for the SPI that will be called from the interrupt
	// context when an SPI status occurs, specify a pointer to the SPI
	// driver instance as the callback reference so the handler is able to
	// access the instance data.
	XSpi_SetStatusHandler(&SpiInstance, &SpiInstance, (XSpi_StatusHandler)SpiIntrHandler);


	// Set the SPI device to the correct mode for this application
	printf("Setting the SPI device into Master mode...");
	Status = XSpi_SetOptions(&SpiInstance, XSP_MASTER_OPTION + XSP_MANUAL_SSELECT_OPTION + XSP_CLK_PHASE_1_OPTION);
	if (Status != XST_SUCCESS) return XST_FAILURE;
	printf("DONE!!\n\r");

	// Select the SPI Slave.  This asserts the correct SS bit on the SPI bus
	XSpi_SetSlaveSelect(&SpiInstance, 0x01);


	// Start the SPI driver so that interrupts and the device are enabled.
	printf("Starting the SPI driver, enabling interrupts and the device...");
	XSpi_Start(&SpiInstance);
	printf("DONE!!\n\r");


	printf("\n\r\n\r");
	printf("Writing to the MAX31723 Config Register...");

	// Clear the SPI read and write buffers
	clear_SPI_buffers();

	// Put the commands for the MAX31723 device in the write buffer
	WriteBuffer[0] = <Set the hexadecimal address for the MAX31723's config register here>
	WriteBuffer[1] = MAX31723_DISABLE_ONE_SHOT_TEMPERATURE_CONVERSION +
			MAX31723_COMPARATOR_MODE +
			MAX31723_THERMOMETER_RESOLUTION_9BIT_MODE +
			MAX31723_CONTINUOUS_TEMPERATURE_CONVERSION_MODE;  // Set the CONFIG register to a basic state

	// Transmit the data.
	SPI_TransferInProgress = TRUE;
	Status = XSpi_Transfer(&SpiInstance, WriteBuffer, NULL, 2);

	while (SPI_TransferInProgress);  // Wait here until the SPI transfer has finished
	printf("DONE!\n\r\n\r\n\r");

	// An endless loop which reads and displays the current temperature
	while(1)
	{
		temperature = read_current_temperature(&SpiInstance);

		// Check to see if the temperature is different from the last reading.
		// Only update the display on the UART if it is different.
		if (previous_temperature != temperature)
		{
			printf("Temperature = %3.4f  {Temperature calibration offset = %3.4f}\n\r", temperature, TEMPERATURE_CALIBRATION_OFFSET);
			previous_temperature = temperature;
		}
	}


	// Disable and disconnect the interrupt system.
	XScuGic_Disconnect(&IntcInstance, XPAR_FABRIC_AXI_QUAD_SPI_0_IP2INTC_IRPT_INTR);


	return XST_SUCCESS;
}



void SpiIntrHandler(void *CallBackRef, u32 StatusEvent, u32 ByteCount)
{
	//printf("** In the SPI Interrupt handler **\n\r");
	//printf("Number of bytes transferred, as seen by the handler = %d\n\r", ByteCount);

	// Indicate the transfer on the SPI bus is no longer in progress
	// regardless of the status event.
	if (StatusEvent == XST_SPI_TRANSFER_DONE)
	{
		SPI_TransferInProgress = FALSE;
	}
	else	// If the event was not transfer done, then track it as an error.
	{
		printf("\n\r\n\r ** SPI ERROR **\n\r\n\r");
		SPI_Error_Count++;
	}
}


void display_buffers(void)
{
	int i;
	for(i=0; i<BUFFER_SIZE; i++)
	{
		printf("Index 0x%02X  -->  Write = 0x%02X  |  Read = 0x%02X\n\r", i, WriteBuffer[i], ReadBuffer[i]);
	}
}

void clear_SPI_buffers(void)
{
	int SPI_Count;

	// Initialize the write buffer and read buffer to zero
	for (SPI_Count = 0; SPI_Count < BUFFER_SIZE; SPI_Count++)
	{
		WriteBuffer[SPI_Count] = 0;
		ReadBuffer[SPI_Count] = 0;
	}

}

float read_current_temperature(XSpi *SpiInstance)
{
	u8 Temperature_LSB = 0;
	u8 Temperature_MSB = 0;
	float Temperature_LSB_float = 0;
	float Temperature_MSB_float = 0;
	float Temperature_float = 0;
	int Status = 0;
	int i = 0;

	// Clear the SPI read and write buffers
	clear_SPI_buffers();

	// Put the commands for the MAX31723 device in the write buffer
	WriteBuffer[0] = <Add values in here to read data from the MAX31723's Temperature MSB register>
	WriteBuffer[1] = <Add values in here to read data from the MAX31723's Temperature MSB register>

	// Transmit the data.
	SPI_TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiInstance, WriteBuffer, ReadBuffer, 2);

	while (SPI_TransferInProgress);  // Wait here until the SPI transfer has finished

	// Fetch the byte of data from the ReadBuffer
	Temperature_MSB = ReadBuffer[1];

	// Clear the SPI read and write buffers
	clear_SPI_buffers();

	// Put the commands for the MAX31723 device in the write buffer
	WriteBuffer[0] = <Add values in here to read data from the MAX31723's Temperature LSB register>
	WriteBuffer[1] = <Add values in here to read data from the MAX31723's Temperature LSB register>

	// Transmit the data.
	SPI_TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiInstance, WriteBuffer, ReadBuffer, 2);
	if (Status != XST_SUCCESS) return XST_FAILURE;

	while (SPI_TransferInProgress);  // Wait here until the SPI transfer has finished

	// Fetch the byte of data from the ReadBuffer
	Temperature_LSB = ReadBuffer[1];

	if (Temperature_MSB & 0x80)  // If the sign bit is a '1'
	{
		Temperature_LSB_float = (float)Temperature_LSB;

		Temperature_MSB = (~Temperature_MSB) + 1;
		Temperature_MSB_float = 0 - (float)Temperature_MSB;
		Temperature_LSB_float = 0;
		for (i=0; i<4; i++)
		{
			if (Temperature_LSB & (0x80 >> i))
			{
				Temperature_LSB_float += 0.5 / pow(2, i);  // For this to work, the -lm switch must be added to the linker command line
			}
		}
	}
	else
	{
		Temperature_LSB_float = (float)Temperature_LSB / 256;
		Temperature_MSB_float = (float)Temperature_MSB;
	}
	Temperature_float = Temperature_MSB_float + Temperature_LSB_float + TEMPERATURE_CALIBRATION_OFFSET;

	return (Temperature_float);
}
