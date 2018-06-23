/*
Author: Anthony SuVasquez
 */

#ifndef POLLING_H_
#define POLLING_H_

#define motorDriveDelay     4
#define valveOpenDelay      5
#define samplingDelay       15

#define VCC             3.3f

#define maxSamples      50 //Maximum samples in the moving average filter
#define ADCconversion   VCC / 1024 //This converts ADC ints into decimals
#define orpOffset       800 //This is just a calibration offset. A magic number
#define muxOffset       25 //In case the mux applies a DC offset to the ADC samples

// Function prototypes
void ConfigureADC(void);
void ADC_off();
int ADC_read();
int movingAvg(int *ptrArrNumbers, long *sum, unsigned int pos, int len, int nextNum);
int poll_turb();
int poll_ORP();
void mux_high();
void mux_low();
void water_pump_on();
void water_pump_off();
void solenoid_on();
void solenoid_off();
void clearSampleBuffers();

#endif /* POLLING_H_ */
