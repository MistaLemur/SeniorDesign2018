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

#ifndef LIB_RNG_H_
#define LIB_RNG_H_

extern uchar max_bits;

sint get_rand(sint,sint);
uint get_seed(uchar);
void init_ADC(uchar,uchar);
void ADC_start(void);

#endif /*LIB_RNG_H_*/
