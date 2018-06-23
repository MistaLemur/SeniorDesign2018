/*
Author: Anthony SuVasquez

This contains all the pin definitions on the same file.
*/
#ifndef PINS_H_
#define PINS_H_


//channel 1
#define adcPin          BIT1

#define CS              0x0010      // chip select at P1.4  (SS1)
#define SDIN            0x0080      // data in at P1.7      (MOSI)
#define SCLK            0x0020      // clk at P1.5          (CLK)
#define DC              0x0008      // Data/Cmd at P1.3     (DC)
#define RES             0x0001      // reset at P1.0        (RES)

#define SDO             0x0040      // data out at P1.6     (MISO)

//channel 2
#define muxPin          BIT3
#define pumpPin         BIT4
#define valvePin        BIT5
#define marxPin         BIT0
#define CS2             0x0004      // chip select2 at P2.2 (SS2)

#define IRQ             0x0002      // Interrupt at P2.1      (IRQ)







#endif
