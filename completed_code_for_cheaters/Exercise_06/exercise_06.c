#include <stdio.h>
#include "platform.h"
#include "xil_io.h"
#include "xparameters.h"


int main()
{
	init_platform();

	int word1;
	int word2;
	int word3;

	printf("Exercise 5\n\r");

	Xil_Out8(XPAR_PS7_RAM_0_S_AXI_BASEADDR + 0x00,0xAB);
	Xil_Out8(XPAR_PS7_RAM_0_S_AXI_BASEADDR + 0x01,0xFF);
	Xil_Out8(XPAR_PS7_RAM_0_S_AXI_BASEADDR + 0x02,0x34);
	Xil_Out8(XPAR_PS7_RAM_0_S_AXI_BASEADDR + 0x03,0x8C);
	Xil_Out8(XPAR_PS7_RAM_0_S_AXI_BASEADDR + 0x04,0xEF);
	Xil_Out8(XPAR_PS7_RAM_0_S_AXI_BASEADDR + 0x05,0xBE);
	Xil_Out8(XPAR_PS7_RAM_0_S_AXI_BASEADDR + 0x06,0xAD);
	Xil_Out8(XPAR_PS7_RAM_0_S_AXI_BASEADDR + 0x07,0xDE);

	Xil_Out32(0xE000A204,0x00000000);
	Xil_Out32(XPAR_PS7_RAM_0_S_AXI_BASEADDR + 0x0C,0x00000000);

	Xil_Out16(XPAR_PS7_RAM_0_S_AXI_BASEADDR + 0x10,0x1209);
	Xil_Out16(XPAR_PS7_RAM_0_S_AXI_BASEADDR + 0x12,0xFE31);
	Xil_Out16(XPAR_PS7_RAM_0_S_AXI_BASEADDR + 0x14,0x6587);
	Xil_Out16(XPAR_PS7_RAM_0_S_AXI_BASEADDR + 0x16,0xAAAA);

	word1 = Xil_In32(XPAR_PS7_RAM_0_S_AXI_BASEADDR + 0x00);
	word2 = Xil_In32(XPAR_PS7_RAM_0_S_AXI_BASEADDR + 0x04);
	word3 = Xil_In32(0xE000A064);

	printf("Word1 = 0x%08x\n\r", word1);
	printf("Word2 = 0x%08x\n\r", word2);
	printf("Word3 = 0x%08x\n\r", word3);

	cleanup_platform();

	return 0;
}
