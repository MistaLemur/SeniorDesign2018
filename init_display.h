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
 ********************************/ 

#ifndef INIT_DISPLAY_H_
#define INIT_DISPLAY_H_

void init_LCD(uint);
void init_USCI(void);

/*
	set_instruction(0,0xCB);	// POWER CONTROL A
	set_instruction(1,0x39);	// 1. (default)
	set_instruction(1,0x2C);	// 2. (default)
	set_instruction(1,0x00);	// 3. (default)
	set_instruction(1,0x34);	// 4. (Vcore Controll: 1.55V, default)
	set_instruction(1,0x02);	// 5. (DDVDH: 5.8V, default)
    
	set_instruction(0,0xCF);	// POWER CONTROL B
	set_instruction(1,0x00);	// 1. (default)
	set_instruction(1,0xC1);	// 2. (PC&EQ operation for power saving enabled), 0x81 def.
	set_instruction(1,0x30);	// 3. (default)
    
	set_instruction(0,0xE8);	// DRIVER TIMING CONTROL A
	set_instruction(1,0x85);	// 1. (gate driver non-overlap timing control), 0x84 def.
	set_instruction(1,0x00);	// 2. (EQ timing control), 0x11 def.
	set_instruction(1,0x78);	// 3. (pre-charge timing control), 0x7A def.
    
	set_instruction(0,0xEA);	// DRIVER TIMING CONTROL A
	set_instruction(1,0x00);	// 1. (gate driver timing control), 0x66 def.
	set_instruction(1,0x00);	// 2. (default)
    
	set_instruction(0,0xED);	// POWER ON SEQUENCE CONTROL
	set_instruction(1,0x64);	// 1. (soft start control), 0x55 def.
	set_instruction(1,0x03);	// 2. (power on sequence control), 0x01 def.
	set_instruction(1,0x12);	// 3. (power on sequence control), 0x23 def.
	set_instruction(1,0x81);	// 4. (DDVDH enhance mode(only for 8 external capacitors)), enabled, 0x01 def.
    
	set_instruction(0,0xF7);	// PUMP RATION CONTROL
	set_instruction(1,0x20);	// 1. (ratio control)
    
	set_instruction(0,0xC0); 	// POWER CONTROL 1
	set_instruction(1,0x23); 	// 1. (set the GVDD level, 4.6V), 0x21 def.
    
	set_instruction(0,0xC1); 	// POWER CONTROL 2
	set_instruction(1,0x10); 	// 1. (sets the factor used in the step-up circuits) default
    
	set_instruction(0,0xC5); 	// VCOM CONTROL 1
	set_instruction(1,0x3e); 	// 1. (VCOMH voltage = 5.85V), 0x31 def.
	set_instruction(1,0x28);	// 2. (VCOML voltage = -1.50V), 0x3C def.
    
	set_instruction(0,0xC7); 	// VCOM CONTROL 2
	set_instruction(1,0x86); 	// 1. (VCOM offset voltage), 0xC0 def.
    
	set_instruction(0,0x36); 	// MEMORY ACCESS CONTROL
	set_instruction(1,0x80); 	// 1. ( ), 0x00 def.
    
	set_instruction(0,0x3A);	// COLMOD: PIXEL FORMAT SET
	set_instruction(1,0x55);	// 1. (sets the pixel format for the RGB image data used by the interface, 16bits/pixel)
    
	set_instruction(0,0xB1);	// FRAME RATE CONTROL (IN NORMAL MODE / FULL COLORS)
	set_instruction(1,0x00);	// 1. (division ratio for internal clocks when Normal mode), default
	set_instruction(1,0x18);	// 2. (frame frequency, 79Hz), 0x1B def. (70Hz)
    
	set_instruction(0,0xB6); 	// DISPLAY FUNCTION CONTROL
	set_instruction(1,0x08);	// 1. (), 0x0A def.
	set_instruction(1,0x82);	// 2. (default)
	set_instruction(1,0x27);	// 3. (default)
    
	set_instruction(0,0xF2); 	// 3GAMMA CONTROL
	set_instruction(1,0x00);	// 1. (disabled), 0x02 def.
    
	set_instruction(0,0x26);	// GAMMA SET
	set_instruction(1,0x01);	// 1. (default)
    
	set_instruction(0,0xE0); 	// POSITIVE GAMMA CORRECTION
	set_instruction(1,0x0F);	// 1.
	set_instruction(1,0x31);	// 2.
	set_instruction(1,0x2B);	// 3.
	set_instruction(1,0x0C);	// 4.
	set_instruction(1,0x0E);	// 5.
	set_instruction(1,0x08);	// 6.
	set_instruction(1,0x4E);	// 7.
	set_instruction(1,0xF1);	// 8.
	set_instruction(1,0x37);	// 9.
	set_instruction(1,0x07);	// 10.
	set_instruction(1,0x10);	// 11.
	set_instruction(1,0x03);	// 12.
	set_instruction(1,0x0E);	// 13.
	set_instruction(1,0x09);	// 14.
	set_instruction(1,0x00);	// 15.
    
	set_instruction(0,0xE1); 	// NEGATIVE GAMMA CORRECTION
	set_instruction(1,0x00);	// 1.
	set_instruction(1,0x0E);	// 2.
	set_instruction(1,0x14);	// 3.
	set_instruction(1,0x03);	// 4.
	set_instruction(1,0x11);	// 5.
	set_instruction(1,0x07);	// 6.
	set_instruction(1,0x31);	// 7.
	set_instruction(1,0xC1);	// 8.
	set_instruction(1,0x48);	// 9.
	set_instruction(1,0x08);	// 10.
	set_instruction(1,0x0F);	// 11.
	set_instruction(1,0x0C);	// 12.
	set_instruction(1,0x31);	// 13.
	set_instruction(1,0x36);	// 14.
	set_instruction(1,0x0F);	// 15.
*/

#endif /*INIT_DISPLAY_H_*/
