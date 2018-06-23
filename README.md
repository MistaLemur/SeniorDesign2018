# SeniorDesign2018

Author: Anthony "Miguel" SuVasquez

This is microcontroller code for a Texas Instruments MSP430G2 microcontroller.
This is being developed on CodeComposerStudio v6

Much of the code is based on a library written by XDEC for the TFT touchscreen driver and the MSP430G2 microcontroller.
The library handles much of the SPI communications for drawing things to the touchscreen, but I have modified it to add SPI communications for polling the touchscreen for touch coordinates, and added a set of interrupt vectors for touchscreen events and SPI receiving events. I have also modified it to use different GPIO pins for the SPI communications. I also wrote my own code for integer->string conversion, and X,Y coordinate -> string conversion for writing number values to the screen.

Additionally, I added code for polling analog water quality sensors. These make use of the on-board ADC, and use an analog multiplexer to switch between sensor channels. The ADC functions by activating the interrupt and selecting a GPIO pin, and then reading from the ADC buffer when the interrupt triggers.
