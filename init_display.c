/*******************************
 *          __                   
 *         /\ \                  
 *  __  _  \_\ \     __    ___   
 * /\ \/'\ /'_' \  /'__'\ /'___\ 
 * \/>  <//\ \ \ \/\  __//\ \__/ 
 *  /\_/\_\ \___,_\ \____\ \____\
 *  \//\/_/\/__,_ /\/____/\/____/
 * 
 *  Author: declis (xdec.de)
 
 Modified by: Anthony SuVasquez. 
 My modifications include: 
 adding the interrupt vector on port 2 for touch events.
 adding interrupt vectors for SPI communications for touchscreen polling.
 changing SPI pins for touchscreen drawing.
 modifying intialization functions to support a second set of SPI pins and interrupts
 modififying initialization functions to create the new interrupt vectors.
 ********************************/ 

#include <msp430g2553.h>
#include "typedef.h"
#include "lib_lcd.h"
#include "delay.h"
#include "pins.h"

// see init_display.h for command description
const uchar init_cmd[]={0xCB,0xCF,0xE8,0xEA,0xED,0xF7,0xC0,0xC1,0xC5,0xC7,0x36,0x3A,0xB1,0xB6,0xF2,0x26,0xE0,0xE1};
const uchar init_data_count[]={5,3,3,2,4,1,1,1,2,1,1,1,2,3,1,1,15,15};
const uchar init_data[]={0x39,0x2C,0x00,0x34,0x02,
						0x00,0xC1,0x30,
						0x85,0x00,0x78,
						0x00,0x00,
						0x64,0x03,0x12,0x81,
						0x20,
						0x23,
						0x10,
						0x3E,0x28,
						0x86,
						0x88,
						0x55,
						0x00,0x18,
						0x08,0x82,0x27,
						0x00,
						0x01,
						0x0F,0x31,0x2B,0x0C,0x0E,0x08,0x4E,0xF1,0x37,0x07,0x10,0x03,0x0E,0x09,0x00,
						0x00,0x0E,0x14,0x03,0x11,0x07,0x31,0xC1,0x48,0x08,0x0F,0x0C,0x31,0x36,0x0F};
						
void init_LCD(uint b_color)
{
	uchar byte=0,bit_num=0,h_index=0,w_index=0;

	// toggle the reset pin on/off a few times to reset the LCD
	P1OUT|=RES;
	wait_ms(10);
	P1OUT&=~RES;
	wait_ms(10);
	P1OUT|=RES;
	wait_ms(100);
	
	while(byte<sizeof(init_cmd))						// byte is init_cmd index
	{
		set_instruction(0,init_cmd[byte++]);			
		while(h_index<init_data_count[bit_num])			// h_index is size of init_data command part counter
		{												// bit_num is init_data_count index
			set_instruction(1,init_data[w_index++]);	// w_index is init_data index
			h_index++;
		}
		bit_num++;
		h_index=0;
	}
    
	set_instruction(0,0x11); 	// SLEEP OUT
	wait_ms(120);				// 120ms needed (see description of sleep-out-cmd)
	
	MEM_WRITE;
    fill_display(lcd_width,lcd_height,b_color);
    DISPLAY_ON;
    MEM_WRITE;
}

void init_USCI(void)
{
	P1DIR|=RES+DC+CS;               //setting pins RES DC and CS as outputs
	P1OUT&=~RES+DC+CS;                  //Setting output pins to 0

	P1SEL|=SDIN+SCLK+SDO;               //Disabling GPIO for SDIN (MISO) and SCLK and SDO (MOSI)
	P1SEL2|=SDIN+SCLK+SDO;              //Disabling GPIO for SDIN (MISO) and SCLK and SDO (MOSI)
	
  	UCB0CTL1 |= UCSWRST;					// USCI in reset state
  	// SPI Master, 8bit, MSB first, synchronous mode
  	UCB0CTL0 |= UCMST + UCSYNC + UCCKPH + UCMSB; //3 pin 8 bit spi masster
  	UCB0CTL1 |= UCSSEL_2;					// USCI CLK-SRC=SMCLK=~8MHz
  	UCB0CTL1 &= ~UCSWRST;					// USCI released for operation
  	IE2|=UCB0TXIE;						// enable TX interrupt
    IE2|=UCB0RXIE;                      // enable RX interrupt

  	IFG2&=~UCB0TXIFG;
    IFG2&=~UCB0RXIFG;

  	_EINT();							// enable interrupts
}

void init_touchScreen(void){
    //Initialize touchscreen stuff
    P2IE &= 0x00;
    P2IES &= 0x00;
    P2REN &= 0x00;
    P2OUT &= 0x00;

    P2OUT &= 0x00;
    P2DIR &= 0x00;
    P2DIR |= CS2;                   // initialize CS2 pins

    //initialize Port2 interrupt
    P2REN |= IRQ;            //Enable internal pull-up/down resistors
    P2OUT |= IRQ;           //Select pull-up mode for IRQ

    P2IFG &= ~IRQ;          //Clear IRQ interrupt flag
    //P2IES &= ~IRQ;           //lo>hi edge
    P2IES |= IRQ;           //hi>lo edge
    P2IE |= IRQ;            //Enable interrupt vector
}

#pragma INTERRUPT (USCI)
#pragma vector=USCIAB0TX_VECTOR
void USCI(void)
{
	P1OUT|=CS;				// transmission done
	P2OUT|=CS2;
	IFG2&=~UCB0TXIFG;		// clear TXIFG
    IFG2&=~UCB0RXIFG;       // clear RXIFG
}	

#pragma INTERRUPT (USCI2)
#pragma vector=USCIAB0RX_VECTOR
void USCI2(void)
{
    P1OUT|=CS;              // transmission done
    P2OUT|=CS2;
    IFG2&=~UCB0TXIFG;       // clear TXIFG
    IFG2&=~UCB0RXIFG;       // clear RXIFG
}

//Port 2 service interrupt
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    //poll x and y
    //take note of the time
    if(P2IFG & IRQ){
        newTouch = 1;
    }

    P2IFG &= ~IRQ;

    __bis_SR_register(GIE);
}
