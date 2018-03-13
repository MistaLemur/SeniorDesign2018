#include <msp430.h>
#include <stdio.h>

/*
 * main.c
 * This code features ADC (Analog-Digital Conversion) polling
 * functions to operate the turbidity and ORP sensors. The functions
 * also include the mux operation for sensor selection
 *
 */


// Global variables
int *sample = {0};


// Function prototypes
void ConfigureADC(void);
int ADC_read();
int movingAvg();
int run_turb();
int run_ORP();
void mux_high();
void mux_low();
void water_pump_on();
void water_pump_off();
void solenoid_on();
void solenoid_off();

void main(void)
{
    P3SEL = 0;                  // I just did this cuz the console recommended to set unused ports to 0
    int turb_reading = 0;       // Initialize turbidity sensor variable to be used in microcontroller operations
    int ORP_reading = 0;        // Initialize ORP sensor variable to be used in microcontroller operations

    turb_reading = run_turb();  // Run turbidity ADC polling, apply moving average filter to array, and return average turbidity value
    ORP_reading = run_ORP();    // Run ORP ADC polling, apply moving average filter to array, and return average ORP value
}


//Function that reads sensor data from the turbidity sensor and stores the average value in MSP430 memory.
int run_turb()
{

    mux_low();                          // Set mux to 0 to access turbidity sensor

    int turb_size = 500;                // Size of the array storing turbidity values. I picked it arbitrarily.
    int turb_window[50] = {0};          // 50 is just a random guess at this point, needs to be adjusted by testing.
    int *sample_turb = {0};             // Initialize sample array for turbidity sensor polling

    ADC_read(turb_size, sample_turb);   // Run ADC read on turbidity sensor

    long sum = 0;                               // Moving avg variable initializations
    int pos = 0;                                                    //
    int turb_avg = 0;                                               //
    int len = sizeof(turb_window) / sizeof(int);                    //

    // For loop which runs moving average filter on the entire span of turbidity reading sample.
    int i = 0;
    for(i = turb_size - 1; i >= 0; i--){
        turb_avg = movingAvg(turb_window, sum, pos, len, sample_turb); // Run Moving average filter on turbidity sensor. Function is based off this example: https://gist.github.com/bmccormack/d12f4bf0c96423d03f82
        pos++;
        if (pos >= len){
            pos = 0;
            }
        }
    return turb_avg;                    // Function returns the average value of the turbidity sensor
}


//Function that reads sensor data from the ORP sensor and stores the average value in MSP430 memory.
int run_ORP()
{

    mux_high();                         // Set mux to high to select ORP sensor

    int ORP_size = 2500;                // Size of the array storing ORP values. This value was picked at random
    int ORP_window[200] = {0};          // 200 is just a random guess at this point. Needs to be adjusted by testing.
    int *sample_ORP = {0};              // Initialize sample array for ORP sensor polling

    ADC_read(ORP_size, sample_ORP);     // Run ADC read on ORP sensor

    long sum = 0;                               // Moving avg variable initializations
    int pos = 0;                                                //^
    int ORP_avg = 0;                                            //^
    int len = sizeof(ORP_window) / sizeof(int);                 //^

    // For loop which runs moving average filter on the entire span of ORP reading sample.
    int i = 0;
    for(i = ORP_size - 1; i >= 0; i--){
        ORP_avg = movingAvg(ORP_window, sum, pos, len, sample_ORP); // Run Moving average filter on turbidity sensor. Function is based off this example: https://gist.github.com/bmccormack/d12f4bf0c96423d03f82
        pos++;
        if (pos >= len){
            pos = 0;
            }
        }
return ORP_avg;                         // Function returns the average value of the ORP sensor.
}





//Function that applies moving average filter to a 1D array of samples. Function is based off this example: https://gist.github.com/bmccormack/d12f4bf0c96423d03f82
int movingAvg(int *ptrArrNumbers, long sum, int pos, int len, int nextNum)
{
      //Subtract the oldest number from the prev sum, add the new number
      sum = sum - ptrArrNumbers[pos] + nextNum;
      //Assign the nextNum to the position in the array
      ptrArrNumbers[pos] = nextNum;
      //return the average
      return sum / len;
}

//Moving Average Filter Function End


//ADC Polling function which polls the sensor and stores values in an array. Function is based off this example: http://coder-tronics.com/msp430-adc-tutorial/
int ADC_read(sample_size, sample)
{

    WDTCTL = WDTPW + WDTHOLD;       // Stop WDT
    BCSCTL1 =   CALBC1_1MHZ;        // Set range   DCOCTL = CALDCO_1MHZ;
    BCSCTL2 &= ~(DIVS_3);           // SMCLK = DCO = 1MHz

    P1SEL |= BIT3;                  // ADC input pin P1.3

    ConfigureADC();                 // ADC set-up function call
    __enable_interrupt();           // Enable interrupts.

    unsigned int i = 0;
    for(i = sample_size; i > 1;  i--)
    {
        __delay_cycles(100);                // Wait for ADC Ref to settle
        ADC10CTL0 |= ENC + ADC10SC;         // Sampling and conversion start
        __bis_SR_register(CPUOFF + GIE);    // Low Power Mode 0 with interrupts enabled
        sample = ADC10MEM;                  // Assigns the value held in ADC10MEM to the integer called ADC_value
    }
    return sample;
}



// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    __bic_SR_register_on_exit(CPUOFF);        // Return to active mode }
}

// Function containing ADC set-up
void ConfigureADC(void)
{

    ADC10CTL1 = INCH_3 + ADC10DIV_3 ;                       // Channel 3, ADC10CLK/3
    ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE;    // Vcc & Vss as reference, Sample and hold for 64 Clock cycles, ADC on, ADC interrupt enable
    ADC10AE0 |= BIT3;                                       // ADC input enable P1.3
}

//End ADC Polling

// Function that sets the multiplexer for sensor selection to high. Current pin configuration is P2.3
void mux_high()
{
    P2DIR |= BIT3;
    P2OUT |= BIT3;
}

// Function that sets the multiplexer for sensor selection to low. Current pin configuration is P2.3
void mux_low()
{
    P2DIR |= BIT3;
    P2OUT &= ~BIT3;
}

// Function that turns on water pump operation / introduces water to testing chamber. Current pin configuration is P2.4
void water_pump_on()
{
    P2DIR |= BIT4;
    P2OUT |= BIT4;
}

// Function that turns off water pump operation / stops adding water to testing chamber. Current pin configuration is P2.4
void water_pump_off()
{
    P2DIR |= BIT4;
    P2OUT &= ~BIT4;
}

// Function that turns on solenoid/opens water drain in the testing chamber. Current pin configuration is P2.5
void solenoid_on()
{
    P2DIR |= BIT5;
    P2OUT |= BIT5;
}

// Function that turns off solenoid/closes water drain in the testing chamber. Current pin configuration is P2.5
void solenoid_off()
{
    P2DIR |= BIT5;
    P2OUT &= ~BIT5;
}
