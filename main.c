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
#include <stdlib.h>
#include "typedef.h"
#include "init_display.h"
#include "lib_math.h"
#include "lib_RNG.h"
#include "lib_lcd.h"
#include "delay.h"
#include "const_data.h"

#define C_BORDER	0x0005
#define s_num		110

void clean_area(uint);
void init_star(uchar);
void get_star_pos(uchar);

uchar space_char=1;		// space between chars
struct point3d_schar dots3d[s_num];

// help vars
uint x3=0;
sint x=0,y=0,x1=0,y1=0,x2=0,y2=0,r=0,i=0,z=0;

void main(void)
{
	WDTCTL=WDTPW+WDTHOLD;		
	BCSCTL1=CALBC1_16MHZ;		// SMCLK=~16MHz  
  	DCOCTL=CALDCO_16MHZ;
  	
  	init_ADC(1,BIT1);			// init. ADC (RNG)
  	init_USCI();				// init. USCI (SPI)
  	init_LCD(C_BLACK);			// init. Display

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

    long timer = 0;
    float dtime = 0;

    uint dataX = 120;
    uint orpY = 70;
    uint turbY = 80;
    uint statusY = 110;
    uint plasmaY = 120;

    uint plasmaState = 0;

    uint inputState = 0;

    uint orpState = 0;
    uint orpThresh = 10;
    uint turbState = 0;
    uint turbThresh = 10;

    uint buttonY = 225;
    uint buttonX = 8;
    uint buttonWidth = lcd_width - buttonX*2;
    uint buttonHeight = 40;


    draw_string(2, orpY, "ORP : ", fontColor, TRANSP, 0, 0);
    draw_string(2, turbY, "TURBIDITY : ", fontColor, TRANSP, 0, 0);
    draw_string(2, statusY, "WATER QUALITY : ", fontColor, TRANSP, 0, 0);
    draw_string(2, plasmaY, "PLASMA STATUS : ", fontColor, TRANSP, 0, 0);


    fill_rect(buttonX, buttonX + buttonWidth, buttonY, buttonY + buttonHeight, C_NAVY);
    draw_rect(buttonX + 2, buttonX + buttonWidth - 4, buttonY + 2, buttonY + buttonHeight - 4, C_WHITE, 0, 0);

    //poll sensors?
    //show timekeeping?
    while(1){
        draw_string(buttonX + buttonWidth/5, buttonY + buttonHeight/3, "ACTIVATE PURIFICATION", C_WHITE, TRANSP, 0, 0);

        uchar buffer[32];

        //poll sensors
/*
        itoa(orpState, buffer, 10);
        draw_string(dataX, orpY, buffer, fontColor, C_BLACK, 0, 0);

        itoa(turbState, buffer, 10);
        draw_string(dataX, turbY, buffer, fontColor, C_BLACK, 0, 0);
        */

        if(orpState > orpThresh && turbState < turbThresh){
            //water is safe?
            draw_string(dataX, statusY, "SAFE", fontColor, C_BLACK, 0, 0);
        }else{
            //water is unsafe?
            draw_string(dataX, statusY, "UNSAFE", fontColor, C_BLACK, 0, 0);
        }
        if(plasmaState == 0){
            draw_string(dataX, plasmaY, "on", C_BLACK, TRANSP, 0, 0);
            draw_string(dataX, plasmaY, "off", fontColor, TRANSP, 0, 0);
        }else{
            draw_string(dataX, plasmaY, "off", C_BLACK, TRANSP, 0, 0);
            draw_string(dataX, plasmaY, "on", fontColor, TRANSP, 0, 0);
        }



        dtime += 0.05;
        if(dtime > 1){
            timer++;
            dtime -= 1;
        }

        wait_ms(50);

    }
  	/*
  	 * The following code block is XDEC's demo code
  	 *
  	// FADE IN HELLO WORLD -------------------------------------------
  	while(x<64)
  	{
  		// fade in via set_RGB function
  		draw_string(90,160,string_00,set_RGB16(0,x++,0),TRANSP,0,0);
  		wait_ms(60);
  	}
  	x=0;
  	// --------------------------------------------------------------- 	
  	
  	// CHAOS RANDOM BINARY CODE --------------------------------------
	while(x++<1500)
		draw_char_text_mode(get_rand(0,39),get_rand(0,51),get_rand(0,1)+48,
								set_RGB16(0,get_rand(0,63),0),C_BLACK,0,0);
								// ASCII offset +48 to get a "0" or "1"
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
	fill_rect(0,239,40,40,C_BORDER+10);		// line
	fill_rect(0,239,280,280,C_BORDER+10);	// line
	// ---------------------------------------------------------------	
	
	wait_ms(1000);
	
	// XDEC Y-ROTATE -------------------------------------------------
	x3=0;x1=300;r=60;
	while(r<200)
	{
		for(x2=0,i=0;x2<4;x2++,i-=90)	// rotate letter dots, y-axis with -90° offset for every letter
			rotate_3dp_array(obj_3d,dots3d,letter_dots_pos[x2][0],letter_dots_pos[x2][1],0,y+i,0,100,r);
		for(x2=0;x2<4;x2++)				// draw letters
			draw_3d_obj(x1,y1,dots3d,obj_edges,letter_edges_pos[x2][0],letter_edges_pos[x2][1],C_YELLOW);
		wait_ms(10);	
		for(x2=0;x2<4;x2++)				// clean letters
			draw_3d_obj(x1,y1,dots3d,obj_edges,letter_edges_pos[x2][0],letter_edges_pos[x2][1],C_BLACK);
		y+=6;					// y-angle
		y1=get_sin(x3)/3+160;
		x3+=5;					// sine index
		if(x1!=120) x1-=2;		// move in from right
		if(x3>1000) r++;		// get smaller
	}
	// ---------------------------------------------------------------	
	
	// ROTATE CUBE (Z-AXIS) + DRAW BMP
	x=-60;y=160;z=0;
	while(x!=120&&z!=360)
	{	
		x+=2;z+=4;
		// draw cube without perspective (2D cube), z-axis
		rotate_3dp_array(obj_3d,dots3d,0,7,0,0,z,0,0);
		draw_3d_obj(x,y,dots3d,obj_edges,0,11,C_WHITE);
		wait_ms(10);
		draw_3d_obj(x,y,dots3d,obj_edges,0,11,C_BLACK);			
	}
	// ---------------------------------------------------------------	
	
	// FADE IN STRING (DECLIS PRESENTS) ------------------------------
	draw_bmap_8bit_v(90,130,pd2,60,60,C_BLACK,C_WHITE,0,0);
	wait_ms(1000);
	x=0;y=0;i=0;r=0;
	while(r!=100)
	{
		draw_string(31+x1-r,43+y1,scroller_01[0],set_RGB16(y,0,0),TRANSP,1,1);
		draw_string(31+x1-r,205+y1,scroller_01[1],C_TEAL,TRANSP,1,0);
		wait_ms(20);
		draw_string(31+x1-r,43+y1,scroller_01[0],C_BLACK,TRANSP,1,1);
		draw_string(31+x1-r,205+y1,scroller_01[1],C_BLACK,TRANSP,1,0);
		if(!i) y++;		// fade in red
		else y--;		// fade out red
		if(y>30) i=1;	
		if(!y) i=0;
		x1=get_sin(x)/8;
		y1=get_sin(2*x)/8;
		x+=10;
		if(x>1000) r++;	// move strings to left
	}
	// ----------------------------------------------------------------
	
	clean_area(C_DARK_BLUE);
	wait_ms(1000);
	
	// RAINBOW LISSA --------------------------------------------------
	x3=380;r=0;y=130;
	while(y>-180)
	{
		for(x=0;x<180;x+=1)
		{
			x1=(get_sin(x)*get_sin(3*x))/100+y;
			y1=(get_sin(x)*get_sin(7*x+r))/100+160;
			draw_pixel_xy(x1,y1,wavelen2rgb(x3),2,1);
		}
		for(x=0;x<180;x+=1)
		{
			x1=(get_sin(x)*get_sin(3*x))/100+y;
			y1=(get_sin(x)*get_sin(7*x+r))/100+160;
			draw_pixel_xy(x1,y1,C_DARK_BLUE,2,1);
		}
		r+=6;				// var for movement effect
		x3+=4;		
		if(x3>800) y-=4;	// for x-pos, move to left
	}
	// ---------------------------------------------------------------	
	
	clean_area(C_DARK_BLUE);
	
	// ELLIPSE CIRCLE EFFECT (LOOKS PRETTY) ------------------------
	z=0;y1=90;r=0;i=5;x=0;
	while(x<4000)
	{
		x+=5;
		y1+=1;
		x1=120+get_sin(y1)/2;
		y=150+get_sin(y1+90)/2;
		z+=2;
		for(y2=0,x2=0;y2<6;y2++,x2+=60)
			draw_circle(x1+get_sin(x+x2)/4,y+get_sin(x+z+x2)/4,i,color_pal[y2],0,0);
		for(y2=0,x2=0;y2<6;y2++,x2+=60)
			draw_circle(x1+get_sin(x+x2)/4,y+get_sin(x+z+x2)/4,i,C_DARK_BLUE+2,0,0);
		if(x>2000)
		{
			if(!r&&z%3) i--;
			if(r&&z%3) i++;
			if(i==1) r=1;
			if(i>15) r=0;
		}
	}
	// ---------------------------------------------------------------
	wait_ms(1000);
	
	// SLOW LISSA DRAWING "BEAUTY OF CODE" ---------------------------
	x3=0;
	while(x3<65500)
	{
		draw_pixel_xy(get_sin(x3)+120,get_sin(10*x3)+160,C_FUCHSIA,0,0);
		x3++;
		if(x3<360) wait_ms(10);
	}
	draw_string(68,76,string_01,C_TEAL,TRANSP,0,1);
	wait_ms(2000);
	// ---------------------------------------------------------------
	
	clean_area(C_DARK_BLUE);
	
	// ROTATING 3D CUBE WITH FONT (YEAH) -----------------------------
	x3=0;x=0;y=0;z=0;i=0;x1=0;r=350;y2=-130;
	while(x3<900)
	{	
		for(x2=0,i=0;x2<4;x2++,i-=90)							// rotate letter dots
			rotate_3dp_array(obj_3d,dots3d,letter_dots_pos[x2][0],letter_dots_pos[x2][1],0,y+i,z,100,r);
		rotate_3dp_array(obj_3d,dots3d,0,7,0,y,z,100,r);		// rotate cube dots
		draw_3d_obj(x1+y2,y1,dots3d,obj_edges,0,11,C_FUCHSIA);	// draw cube
		if(x3>500)	// draw letters on cube
		{
			for(x2=0;x2<4;x2++)
				draw_3d_obj(x1+y2,y1,dots3d,obj_edges,letter_edges_pos[x2][0],letter_edges_pos[x2][1],C_LIME);
		}
		draw_3d_obj(x1+y2,y1,dots3d,obj_edges,0,11,C_DARK_BLUE);
			for(x2=0;x2<4;x2++)
				draw_3d_obj(x1+y2,y1,dots3d,obj_edges,letter_edges_pos[x2][0],letter_edges_pos[x2][1],C_DARK_BLUE);
		y+=5;z+=3;				// rotating angles
		x1=get_sin(x)/2;		
		y1=160+get_sin(3*x)/2;
		x+=4;					// sine wave index
		x3+=1;					// var for animation length
		if(y2!=120) y2+=2;		// move cube in from left side
		if(x3>160&&x3<600&&r>110) r--; 	// grow up cube
		if(x3>700&&r<300) r++; 
	}	
	// ---------------------------------------------------------------
	
	wait_ms(1000);
	
	// RAINBOW SCROLLER ----------------------------------------------
	space_char=4;
	x=120;y=0;y1=0;
	while(x!=-900)
	{
		draw_string_sin(x,75,scroller_00,x3,2,wavelen2rgb(y++),TRANSP,4,1);
		wait_ms(10);
		draw_string_sin(x,75,scroller_00,x3,2,C_DARK_BLUE,TRANSP,4,1);
		x-=1;
		x3+=8;
	}
	// ---------------------------------------------------------------

	wait_ms(1000);
	
	// ANIMATED LISSA WITH CHARS -------------------------------------
	x3=0;r=0;i=1;
	while(x3<250)
	{
		for(x=0;x<300;x+=6)
		{
			x1=get_sin(x)/2+57;
			y1=get_sin(i*x+r)/2+160;
			draw_char(x1,y1,'x',C_TEAL,TRANSP,1,0);
		}
		for(x=0;x<300;x+=6)
		{
			x1=get_sin(x)/2+57;
			y1=get_sin(i*x+r)/2+160;
			draw_char(x1,y1,'x',C_DARK_BLUE,TRANSP,1,0);
		}
		r+=16;
		x3++;
		if(x3>50&&x3<52) i++;
		if(x3>100&&x3<102) i++; 
		if(x3>200&&x3<202) i++; 
	}
	// ---------------------------------------------------------------
	
	clean_area(C_DARK_BLUE);
	
	// GREETINGS -----------------------------------------------------
	space_char=1;
	draw_string(5,50,string_02,C_AQUA,TRANSP,2,0);
	wait_ms(2000);
	
	z=90;y1=90;r=0;i=5;x=0;x1=140;
	while(x<600)
	{
		x+=4;
		y1+=1;
		for(y2=0,x2=0;y2<6;y2++,x2+=60)
			draw_char(37+get_sin(x+x2)/6,45+get_sin(x+z+x2)/6,string_03[y2],color_pal[y2],TRANSP,2,2);
		draw_string(x1,250,string_04,C_FUCHSIA,TRANSP,1,0);
		wait_ms(5);
		for(y2=0,x2=0;y2<6;y2++,x2+=60)
			draw_char(37+get_sin(x+x2)/6,45+get_sin(x+z+x2)/6,string_03[y2],C_DARK_BLUE,TRANSP,2,2);
		draw_string(x1,250,string_04,C_DARK_BLUE,TRANSP,1,0);
		x1-=2;	
	}
	
	space_char=4;
	x=120;y=0;y1=0;
	while(x>-600)
	{
		draw_string_sin(x,75+get_sin(x3)/8,scroller_02,x3,4,C_LIME,TRANSP,3,1);
		wait_ms(10);
		draw_string_sin(x,75+get_sin(x3)/8,scroller_02,x3,4,C_DARK_BLUE,TRANSP,3,1);
		x-=2;
		x3+=4;
	}
	// ---------------------------------------------------------------
	
	clean_area(C_BLACK);
	wait_ms(1000);
	
	// THE END - INFO STRINGS ----------------------------------------
	space_char=1;
	for(x=0;x<3;x++)
		draw_string(8,x*12+4,string_05[x],C_TEAL,TRANSP,0,0);
	draw_string(4,97,string_06,C_TEAL,TRANSP,4,2);
	// ---------------------------------------------------------------
	
	// THE END - STARFIELD EFFECT ------------------------------------
	for(i=0;i<s_num;i++)
		init_star(i);
	x3=0;
	while(1)
	{
		// draw stars
		for(i=0;i<s_num;i++)
		{
			get_star_pos(i);
			if(x>=lcd_width||x<0||y<46||y>274||dots3d[i].z<1)
				init_star(i);
			else draw_pixel_xy(x,y,C_WHITE,0,0);
		}
		wait_ms(5);
		// clean stars
		for(i=0;i<s_num;i++)
		{
			get_star_pos(i);
			if(x<lcd_width&&x>=0&&y>45&&y<275&&dots3d[i].z>1)
				draw_pixel_xy(x,y,C_BLACK,0,0);
		}
		// move stars closer
		for(i=0;i<s_num;i++)
			dots3d[i].z-=4;
	}	
	// ---------------------------------------------------------------
	*/
}

void get_star_pos(uchar pos)
{
	x=dots3d[pos].x*100/dots3d[pos].z+120;
	y=dots3d[pos].y*100/dots3d[pos].z+160;
}

void init_star(uchar pos)
{
	dots3d[pos].x=get_rand(-80,80); 
	dots3d[pos].y=get_rand(-80,80); 
	dots3d[pos].z=get_rand(20,127); 
}

void clean_area(uint color)
{
	x=46;
	while(x++<274)
		fill_rect(0,239,x,x,color);
}
