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
 
 Mofieid by Anthony SuVasquez
 My modifications include:
 intToString() and coordsToString().
 These functions convert integers to string and XY coordinates to a string.
 
 I wrote these because for some reason, the IDE would not import atoc().
 ********************************/ 

#include "typedef.h"
#include "lib_math.h"

// sin(x)*100 (0 - 90°)		
const uchar sine_array[]={0,1,3,5,6,8,10,12,13,15,17,19,20,22,24,25,27,29,30,32,34,35,37,39,40,42,
						43,45,46,48,50,51,52,54,55,57,58,60,61,62,64,65,66,68,69,70,71,73,74,75,76,
						77,78,79,80,81,82,83,84,85,86,87,88,89,89,90,91,92,92,93,93,94,95,95,96,96,
						97,97,97,98,98,98,99,99,99,99,99,99,99,99,100};

uint get_pow(uint base, uint exp)
{
	volatile uint result=1;
	if(!exp) return 1;
	while(exp--) result*=base;
	return result;
}

sint get_sin(sint x)
{
	// normalize x value
	if(x<0) x=360-(get_abs(x)%360);
	else x%=360;
	// 0° - 90°
	if(x<=90) return sine_array[x];
	// 91° - 180°
	else if(x>90&&x<=180) return sine_array[180-x];	
	// 181° - 270°
	else if(x>180&&x<=270) return -sine_array[90-(270-x)];
	// 271° - 360°
	else return -sine_array[360-x];
}

sint get_cos(sint x){return get_sin(x+90);}

uint get_abs(sint x)
{
	if(x<0) x*=-1;
	return x;
}

void intToString(int num, uchar* buffer, int bufferLength){
    int i=0;

    if(num < 0){
        buffer[0] = '-';
        i++;
        num *= -1;
    }

    int numCopy = num;
    int numLength = 0;
    while(numCopy != 0){
        numCopy /= 10;
        numLength++;
    }
    if(numLength == 0) numLength = 1;

    i += numLength-1;

    i++;
    if(i < bufferLength) buffer[i] = 0; //add null terminating char
    i--;

    int digit = 0;
    numCopy = num;

    while(digit < numLength){
        int n = numCopy % 10;

        if(i < bufferLength) buffer[i] ='0' + n;

        numCopy /= 10;
        i--;
        digit++;
    }

}


void coordsToString(int x, int y, uchar* buffer, int bufferLength){
    int i=0;

    if(x < 0){
        buffer[i] = '-';
        i++;
        x *= -1;
    }

    int numCopy = x;
    int numLength = 0;
    while(numCopy != 0){
        numCopy /= 10;
        numLength++;
    }
    if(numLength == 0) numLength = 1;

    i += numLength-1;

    i++;
    if(i < bufferLength) buffer[i] = ','; //add null terminating char
    i--;

    int digit = 0;
    numCopy = x;

    while(digit < numLength){
        int n = numCopy % 10;

        if(i < bufferLength) buffer[i] ='0' + n;

        numCopy /= 10;
        i--;
        digit++;
    }

    i = numLength + 1;
    if(x < 0) i++;

    //y coord

    if(y < 0){
        buffer[i] = '-';
        i++;
        y *= -1;
    }

    numCopy = y;
    numLength = 0;
    while(numCopy != 0){
        numCopy /= 10;
        numLength++;
    }
    if(numLength == 0) numLength = 1;

    i += numLength-1;

    i++;
    if(i < bufferLength) buffer[i] = 0; //add null terminating char
    i--;

    digit = 0;
    numCopy = y;

    while(digit < numLength){
        int n = numCopy % 10;

        if(i < bufferLength) buffer[i] ='0' + n;

        numCopy /= 10;
        i--;
        digit++;
    }


}

int strLen(char* string, int maxLength){
    int i = 0;
    while(string[i] != 0 && i < maxLength){
        i++;
    }
    return i;
}

void strCat(char* str1, char* str2, int maxLength){ //this funciton concatenates str2 onto str1

    int i = 0;
    int j = 0;
    while(i < maxLength && str1[i] != 0){
        i++;
    }

    while(j < maxLength && i < maxLength && str2[j] != 0){
        str1[i] = str2[j];
        i++;
        j++;
    }
    if(i < maxLength)
        str1[i] = 0;
}
