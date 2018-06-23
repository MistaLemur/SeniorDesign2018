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
 
Modified by Anthony SuVasquez
 ********************************/ 

#ifndef LIB_MATH_H_
#define LIB_MATH_H_

#define sin_amp_factor		100		// sin(x)*100, for integer calculation

uint get_pow(uint,uint);
sint get_sin(sint);
sint get_cos(sint);
uint get_abs(sint);

void intToString(int, uchar*, int);
void coordsToString(int, int, uchar*, int);

int strLen(char* string, int maxLength);
void strCat(char* str1, char* str2, int maxLength);

#endif /*LIB_MATH_H_*/
