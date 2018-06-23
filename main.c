/*******************************
* Author: Anthony SuVasquez
 ********************************/ 

#include <msp430g2553.h>
#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "init_display.h"
#include "lib_math.h"
#include "lib_lcd.h"
#include "delay.h"
#include "const_data.h"
#include "polling.h"
#include "pins.h"

#define C_BORDER	0x0005
#define s_num		110

void clean_area(uint);
void init_star(uchar);
void get_star_pos(uchar);

int newTouch = 0;
int touchX=0, touchY=0;
long touchTime=0;
float touchDTime=0;

uchar space_char=1;		// space between chars
struct point3d_schar dots3d[s_num];

// help vars
uint x3=0;
sint x=0,y=0,x1=0,y1=0,x2=0,y2=0,r=0,i=0,z=0;

long timer = 0;
float dtime = 0;
const float delay = 1.0/60.0;
float touchDelay = 0.0;

void main(void)
{
	WDTCTL = WDTPW+WDTHOLD;		//stop watchdog timer
	BCSCTL1 = CALBC1_16MHZ;		// SMCLK=~16MHz   setting clock BCSCTL1
  	DCOCTL = CALDCO_16MHZ;      	//setting clock DCOCTL1
  	
  	init_USCI();				// init. USCI (SPI)
  	init_LCD(C_BLACK);			// init. Display

    P2DIR |= pumpPin + valvePin + muxPin + marxPin;

    while(++x<64)
    {
        // fade in via set_RGB function
        draw_string(75, 160, "HELIO-SAFE 2018", set_RGB16(0,x/4,x), TRANSP, 0, 0);
        wait_ms(60);
    }
    x=0;

    //create the screen:
    //backdrop
    //plasma system status
    //
    //sensor readings status

    x=0;
    // ---------------------------------------------------------------

    // CREATE BORDERS ------------------------------------------------
    fill_display(lcd_width,lcd_height,C_BORDER);
    x=lcd_height/2;y=lcd_height/2;
    while(x!=45)
    {
        fill_rect(0,239,x,x,C_BLACK);
        fill_rect(0,239,x-1,x-1,C_BORDER+10);
        x--;
        fill_rect(0,239,y,y,C_BLACK);
        fill_rect(0,239,y+1,y+1,C_BORDER+10);
        y++;
        wait_ms(10);
    }



    x=0;y=0;


    uint fontColor = set_RGB16(0,15,63);

    draw_string(14, 9, "HELIO-SAFE 2018", fontColor, TRANSP, 1, 1);

    const uint dataX = 120;
    const uint orpY = 70;
    const uint turbY = 80;
    const uint statusY = 110;
    const uint plasmaY = 120;

    uint plasmaState = 0;
    uint pollingState = 0;
    uint lastPlasmaState = -1;
    uint lastWaterSafe = -1;
    int lastTouchState = 0;

    int plasmaStateDelay = 30;
    float lastPlasmaPercent = 0;

    uint inputState = 0;

    int orpState = 0;
    int lastORPState = -1;
    const int orpThresh = 665;
    int turbState = 0;
    int lastTurbState = -1;
    const int turbThresh = 1;
    int safeCount = 0;
    const int safeThresh = 600;

    const uint buttonY = 225;
    const uint buttonX = 8;
    const uint buttonWidth = lcd_width - buttonX*2;
    const uint buttonHeight = 40;


    draw_string(2, orpY, "ORP : ", fontColor, TRANSP, 0, 0);
    draw_string(2, turbY, "TURBIDITY : ", fontColor, TRANSP, 0, 0);
    draw_string(2, statusY, "WATER QUALITY : ", fontColor, TRANSP, 0, 0);
    draw_string(2, plasmaY, "PLASMA STATUS : ", fontColor, TRANSP, 0, 0);

    drawPurificationButtonUnpressed(buttonX, buttonY, buttonWidth, buttonHeight, "ACTIVATE PURIFICATION");

    init_touchScreen();

    touchX = 0;
    touchY = 0;

    uchar buffer[64] = {0};
    const int bufferLength = 64;

    //poll sensors?
    //show timekeeping?
    while(1){


        //poll sensors
        //orpState = poll_ORP();
        //turbState = poll_turb();

        if(newTouch){
            //poll the touch sensor...

            newTouch = 0;

	    //If the on-screen button was pressed and the system is idle, activate the plasma system.
            if(plasmaState == 0 && pollingState == 0){
                touchTime = timer;
                touchDTime = dtime;
                drawPurificationButtonPressed(buttonX, buttonY, buttonWidth, buttonHeight, "!! PURIFYING !!");
                plasmaState = 1;
                plasmaOn();
                lastPlasmaPercent = 0;
                fill_rect(buttonX + 4, buttonX + buttonWidth - 8, buttonY + buttonHeight - 11, buttonY + buttonHeight - 7, C_NAVY);
                //set the plasma pin to high
            }
        }


        if(plasmaState != 0){
	    //Run the plasma and update the on-screen timer showing how much time is left
		
            float percent = (timer - touchTime + dtime - touchDTime) * 100.0 / plasmaStateDelay;

            if(lastPlasmaPercent != percent && percent >= 0 && percent <= 100){
                //fill_rect(buttonX + 4, buttonX + (buttonWidth - 8), buttonY + buttonHeight - 11, buttonY + buttonHeight - 7, C_WHITE);
                fill_rect(buttonX + (100 - percent) * (buttonWidth - 8) / 100,
                          buttonX + (100 - lastPlasmaPercent) * (buttonWidth - 8) / 100,
                          buttonY + buttonHeight - 11, buttonY + buttonHeight - 7, C_WHITE);
                //fill_rect(buttonX + 4, buttonX + (100 - percent) * (buttonWidth - 8) / 100, buttonY + buttonHeight - 11, buttonY + buttonHeight - 7, C_NAVY);
            }
            lastPlasmaPercent = percent;

            if(timer > plasmaStateDelay + touchTime){
                plasmaState = 0;
                plasmaOff();

                drawPurificationButtonPressed(buttonX, buttonY, buttonWidth, buttonHeight,  "...Pumping...");
                //run motors for 3 seconds
                solenoid_off();
                water_pump_on();
                wait_ms(1000 * motorDriveDelay);
                water_pump_off();
                drawPurificationButtonPressed(buttonX, buttonY, buttonWidth, buttonHeight, "...Testing...");

                clearSampleBuffers();
                safeCount = 0;

                ConfigureADC();
                pollingState = 60 * samplingDelay;
            }
        }



        if(plasmaState == 0 && lastPlasmaState != 0){
            draw_string(dataX, plasmaY, "on", C_BLACK, TRANSP, 0, 0);
            draw_string(dataX, plasmaY, "off", fontColor, TRANSP, 0, 0);
        }else if(plasmaState == 1 && lastPlasmaState != 1){
            draw_string(dataX, plasmaY, "off", C_BLACK, TRANSP, 0, 0);
            draw_string(dataX, plasmaY, "on", fontColor, TRANSP, 0, 0);
        }

        //polling
        if(pollingState > 0){
	    //After plasma has been run, start polling water quality and update the sensor readouts.
	    //Then determine if the water is safe to drink based on ORP and turbidity readings.
            pollingState--;

            if(pollingState % 2){
                orpState = poll_ORP();
            }else{
                turbState = poll_turb();
            }

            if(pollingState == 0){
                ADC_off();

                drawPurificationButtonPressed(buttonX, buttonY, buttonWidth, buttonHeight,  "...Draining...");
                water_pump_off();
                solenoid_on();
                wait_ms(1000 * valveOpenDelay);
                solenoid_off();
                drawPurificationButtonUnpressed(buttonX, buttonY, buttonWidth, buttonHeight,  "ACTIVATE PURIFICATION");
            }
        }


        if(lastORPState != orpState){
            intToString(orpState, buffer, bufferLength);
            strCat(buffer, " mV     ", bufferLength);
            draw_string(dataX, orpY, buffer, fontColor, C_BLACK, 0, 0);
        }
        if(lastTurbState != turbState){
            intToString(turbState, buffer, bufferLength);
            strCat(buffer, " NTU     ", bufferLength);
            draw_string(dataX, turbY, buffer, fontColor, C_BLACK, 0, 0);
        }


        if(orpState >= orpThresh && turbState <= turbThresh){
            //water is safe?
            safeCount++;
            if(lastWaterSafe != 1 && safeCount > safeThresh){
                draw_string(dataX, statusY, "SAFE    ", fontColor, C_BLACK, 0, 0);
                lastWaterSafe = 1;
            }
        }else {
            safeCount = 0;
            if(lastWaterSafe != 0){
                //water is unsafe?
                draw_string(dataX, statusY, "UNSAFE", fontColor, C_BLACK, 0, 0);
                lastWaterSafe = 0;
            }
        }

        lastTouchState = newTouch;
        lastPlasmaState = plasmaState;
        lastORPState = orpState;
        lastTurbState = turbState;


        dtime += delay;
        if(dtime > 1){
            timer++;
            dtime -= 1;
        }

        wait_ms(delay * 1000);
    }
}

void drawPurificationButtonUnpressed(const int buttonX, const int buttonY, const int buttonWidth, const int buttonHeight, char* string){
    fill_rect(buttonX, buttonX + buttonWidth, buttonY, buttonY + buttonHeight, C_NAVY);
    draw_rect(buttonX + 2, buttonX + buttonWidth - 4, buttonY + 2, buttonY + buttonHeight - 4, C_WHITE, 0, 0);
    draw_string(buttonX + buttonWidth/5, buttonY + buttonHeight/3, string, C_WHITE, TRANSP, 0, 0);
}

void drawPurificationButtonPressed(const int buttonX, const int buttonY, const int buttonWidth, const int buttonHeight, char* string){
    fill_rect(buttonX, buttonX + buttonWidth, buttonY, buttonY + buttonHeight,  C_WHITE);
    draw_rect(buttonX + 2, buttonX + buttonWidth - 4, buttonY + 2, buttonY + buttonHeight - 4, C_NAVY, 0, 0);
    draw_string(buttonX + buttonWidth/4, buttonY + buttonHeight/3, string, C_NAVY, TRANSP, 0, 0);
}

void plasmaOn(){
    P2OUT |= marxPin;

}
void plasmaOff(){
    P2OUT &= ~marxPin;

}

