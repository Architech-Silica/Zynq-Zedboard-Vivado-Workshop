# Zynq Zedboard Vivado Workshop

A Zynq Workshop for Beginners
-----------------------------

A workshop for beginners who are starting to use the Xilinx Zynq SoC devices.  This version of the workshop targets the [Avnet ZedBoard](http://www.zedboard.org).

This workshop requires the [Xilinx Vivado 2014.1 tools](http://www.xilinx.com/support/download.html), or later.

------------


## Contents

1. Getting something (anything!) working.
	+ "I just want to see the board work. How do I do it?"
	+ Use of the Block Diagram tool.
	+ "print" is your friend. We will learn how to use it for debugging.
2. Using drivers to flash an LED.
	+ What is a driver?
	+ Where do I find the supplied drivers?
	+ Using the GPIO peripheral and the supplied drivers, control some pins to flash LEDs on the board.
3. Debugging.
	+ Why is this important?
	+ How does it help us?
	+ What options do we have for debugging?
	+ Use the debugger to step through your Flashing LED design.
4. Expanding your design into the programmable logic (PL).
	+ Add soft peripherals to the AXI interfaces.
	+ Interfacing between the PS and PL.
	+ Connecting interrupts.
	+ Assigning IO pin locations.
5. Making your design interactive.
	+ How do we read inputs from the user?
	+ User input driver functions.
	+ Using a UART connection, make your software respond to characters sent via the UART.
	+ Make an LED light on the board when a certain character is sent from the UART.
6. Talking to internal memory
	+ Use the "Xil_io" functions to write to and read from memory
	+ Why is this important?
7. Timers (Polled mode)
	+ Why are timers useful?
	+ When do we use them?
	+ Using a timer peripheral, flash an LED at a frequency of 1Hz.
8. Timers (Interrupt mode)
	+ What is an interrupt?
	+ Why are they important?
	+ Make a system that will light an LED in response to a user input, but at the same time flash another LED at a frequency of 1Hz. Neither task is permitted to noticeably interfere with the other.
9. Talking to external components.
	+ Using a PMOD expansion module, we will experiment with using the SPI protocol to talk to a temperature sensor.
	+ This exercise will discuss the basics of SPI, and will show how to read from and write to an external device to expand the capabilities of your design.
	+ We will then implement an interrupt based design to send and receive data from the external board via SPI.
10. Autonomous Boot
	+ Using an SD flash memory card, we will make the ZedBoard boot automatically.



Contributions
-------------
Code examples are provided for your use, but please feel free to contribute your own code back to this repository via a pull request in the usual fashion.  Please fork from this repo, then create a suitably named branch in your fork before submitting back to this repo.  Please don't submit a pull request from your "master" branch.  Each new addition to the code should belong to its own submitted branch.  Thanks. 


