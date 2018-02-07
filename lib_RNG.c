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

#include <msp430g2553.h>
#include "typedef.h"
#include "lib_RNG.h"

uchar max_bits=7;	// max. number

/*********************************************************************************
 * Name: 		get_rand
 * Description: - generates a random number between min and max
 * 				- function is based on a linear equation y=mx+b
 * 				- the maximum value can be set via max_bits
 * 				- if you need a number between 0-255, max_bits need to be set to
 * 					8 (2^8), number between 0-4095 -> 12 (2^12) etc.
 * Input:		minimum (min), maximum (max)
 * Return:		random number
 *********************************************************************************/

sint get_rand(sint min, sint max)
{
	//return min+((slong)(max-min+1)*get_seed(max_bits))/(get_pow(2,max_bits)-1);
	// division via shift operation
	return min+(((slong)(max-min+1)*get_seed(max_bits))>>max_bits);
}

/*********************************************************************************
 * Name: 		get_seed
 * Description: - generates a random number between 0 to a max. of (2^16)-1
 * 				- RNG is based on ADC10 and a floating port (noise)
 * 				- maximum value can be set via bit_count
 * 				- if bit_count is set to 16, ADC converison will be started 16x times
 * 					and the LSB of ADC10MEM will be shifted into a word 16x 
 * Input:		bit_count (maximum number)
 * Return:		random number
 *********************************************************************************/

uint get_seed(uchar bit_count)
{
	uint mix_up=0;uchar index=0;
	while(index<bit_count)
	{
		ADC_start();
		// LSB of ADC10MEM will be shifted into a word
		mix_up+=(ADC10MEM&1)<<index++;
	}
	// xor old with new value or something else maybe
	return mix_up;
}

/*********************************************************************************
 * Name: 		init_ADC
 * Description: - initialize ADC10
 * Input:		ADC channel, analog port
 * Return:		nothing
 *********************************************************************************/

void init_ADC(uchar channel, uchar port)
{
	ADC10CTL0&=~ENC;	// conversion disabled
	//Channel 2; ADC10 CLK divider = /8; CLK=ADC10OSC
  	ADC10CTL1=(uint)(channel<<12)+ADC10DIV_7+ADC10SSEL_0;
  	//Vref+=Vcc (3.3V)&Vss=Vr- ; S&H time 16x; ADC10 on         
  	ADC10CTL0=SREF_0+ADC10SHT_3+ADC10ON;  
  	// P1.2 is analog input (floating port)
  	ADC10AE0|=port;                        
}

/*********************************************************************************
 * Name: 		ADC_start
 * Description: - converted value will be in ADC10MEM
 * Input:		nothing
 * Return:		nothing
 *********************************************************************************/

void ADC_start(void)
{
	ADC10CTL0|=ENC+ADC10SC;			// start & enable conversion		
	while(ADC10CTL0&ADC10BUSY);		// ADC busy? 
}
