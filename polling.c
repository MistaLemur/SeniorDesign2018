/*
Author: Anthony SuVasquez
*/
#include <msp430.h>
#include <stdio.h>

/*
 * main.c
 * This code features ADC (Analog-Digital Conversion) polling
 * functions to operate the turbidity and ORP sensors. The functions
 * also include the mux operation for sensor selection
 *
 */

#include "polling.h"
#include "pins.h"

// Global variables
int turbSamples[maxSamples] = {0};
int numTurbSamples = 0;
unsigned int turbIndex = 0;
long turbSum = 0;

int orpSamples[maxSamples] = {0};
int numORPSamples = 0;
unsigned int orpIndex = 0;
long orpSum = 0;


int poll_turb(){ //This polls the turbidity sensor and returns the new turbidity average in terms of NTUs
    mux_low();

    if(++numTurbSamples > maxSamples) numTurbSamples = maxSamples;

    int newSample = ADC_read();   // Run ADC read on turbidity sensor
    int avg = movingAvg(turbSamples, &turbSum, turbIndex, numTurbSamples, newSample);

    if(++turbIndex >= maxSamples) turbIndex = 0;

    //conversion function
    avg = (int) (avg * ADCconversion);

    return avg;
}


int poll_ORP(){ //This polls the ORP and returns the ORP value in terms of millivolts
    mux_high();

    if(++numORPSamples > maxSamples) numORPSamples = maxSamples;

    int newSample = ADC_read();   // Run ADC read on turbidity sensor
    int avg = movingAvg(orpSamples, &orpSum, orpIndex, numORPSamples, newSample);

    if(++orpIndex >= maxSamples) orpIndex = 0;

    //conversion function
    avg = (int)(VCC * 400 - avg * VCC * 1000 / 1024) + orpOffset;

    return avg;
}



//Function that applies moving average filter to a 1D array of samples.
int movingAvg(int *ptrArrNumbers, long *sum, unsigned int pos, int len, int nextNum)
{
    //Subtract the oldest number from the prev sum, add the new number
    *sum += nextNum - ptrArrNumbers[pos];
    //Assign the nextNum to the position in the array
    ptrArrNumbers[pos] = nextNum;
    //return the average
    return *sum / len - muxOffset;
}

//Moving Average Filter Function End


//ADC Polling function which polls the sensor and stores values in an array. Function is based off this example: http://coder-tronics.com/msp430-adc-tutorial/

int ADC_read()
{

    WDTCTL = WDTPW + WDTHOLD;       // Stop WDT
    BCSCTL1 =   CALBC1_1MHZ;        // Set range   DCOCTL = CALDCO_1MHZ;
    BCSCTL2 &= ~(DIVS_3);           // SMCLK = DCO = 1MHz

    P1SEL |= adcPin;                  // ADC input pin P1.3

    __delay_cycles(100);                // Wait for ADC Ref to settle
    ADC10CTL0 |= ENC + ADC10SC;         // Sampling and conversion start
    __bis_SR_register(CPUOFF + GIE);    // Low Power Mode 0 with interrupts enabled

    P1SEL &= ~adcPin;                  // ADC input pin P1.3
    ADC10CTL0 &= ~(ENC + ADC10SC);         // Sampling and conversion start
    return ADC10MEM;                  // Return ADC10MEM
}



// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    __bic_SR_register_on_exit(CPUOFF);        // Return to active mode }
    BCSCTL1 = CALBC1_16MHZ;     // SMCLK=~16MHz   setting clock BCSCTL1
    DCOCTL = CALDCO_16MHZ;      //setting clock DCOCTL1

}

// Function containing ADC set-up
void ConfigureADC(void)
{

    ADC10CTL1 = INCH_1 + ADC10DIV_3 ;                       // Channel 1, ADC10CLK/3
    ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE;    // Vcc & Vss as reference, Sample and hold for 64 Clock cycles, ADC on, ADC interrupt enable

    ADC10AE0 |= adcPin;                           // ADC input enable
}

void ADC_off(){
    ADC10CTL1 = 0;
    ADC10CTL0 = 0;

    ADC10AE0 = 0;
}

//End ADC Polling

// Function that sets the multiplexer for sensor selection to high. Current pin configuration is P2.3
void mux_high()
{
    P2OUT |= muxPin;
}

// Function that sets the multiplexer for sensor selection to low. Current pin configuration is P2.3
void mux_low()
{
    P2OUT &= ~muxPin;
}

// Function that turns on water pump operation / introduces water to testing chamber. Current pin configuration is P2.4
void water_pump_on()
{
    P2OUT |= pumpPin;
}

// Function that turns off water pump operation / stops adding water to testing chamber. Current pin configuration is P2.4
void water_pump_off()
{
    P2OUT &= ~pumpPin;
}

// Function that turns on solenoid/opens water drain in the testing chamber. Current pin configuration is P2.5
void solenoid_on()
{
    P2OUT |= valvePin;
}

// Function that turns off solenoid/closes water drain in the testing chamber. Current pin configuration is P2.5
void solenoid_off()
{
    P2OUT &= ~valvePin;
}

void clearSampleBuffers(){
    int i = 0;
    for(i = 0; i < 50; i++){
        turbSamples[i] = 0;
        orpSamples[i] = 0;
    }
    numTurbSamples = 0;
    turbIndex = 0;
    turbSum = 0;

    numORPSamples = 0;
    orpIndex = 0;
    orpSum = 0;
}
