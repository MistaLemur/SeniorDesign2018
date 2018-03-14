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
#include "ascii_char.h"
#include "lib_lcd.h"
#include "lib_math.h"
#include "delay.h"
#include "pins.h"


uint byte,w_index,h_index;
uchar bit_num;

void draw_3d_obj(sint x, sint y, struct point3d_schar *obj_pts, const uchar edge_data[][2], 
						uchar start, uchar end ,uint col)
{
	for(byte=start;byte<=end;byte++)
		draw_line(obj_pts[edge_data[byte][0]].x+x,obj_pts[edge_data[byte][0]].y+y,
					obj_pts[edge_data[byte][1]].x+x,obj_pts[edge_data[byte][1]].y+y,col,0,0);
}

// rotate an array of 3d_points
void rotate_3dp_array(const schar p3d_rot[][3], struct point3d_schar *p3d_new, uchar start, uchar end,
					sint angle_x, sint angle_y, sint angle_z, sint viewDist, sint viewSize)
{
	struct point3d_sint p3d_const,p3d_sint_new;
	for(byte=start;byte<=end;byte++)
	{
		p3d_const.x=p3d_rot[byte][0];		// load constant points into RAM
		p3d_const.y=p3d_rot[byte][1];
		p3d_const.z=p3d_rot[byte][2];
		rotate_3dp(p3d_const,&p3d_sint_new,angle_x,angle_y,angle_z,viewDist,viewSize);
		p3d_new[byte].x=p3d_sint_new.x;		// save rotated point
		p3d_new[byte].y=p3d_sint_new.y;
		p3d_new[byte].z=p3d_sint_new.z;
	}
}

// p3d_rot = point to rotate , p3d_new = rotated point, viewDist = Perspective
void rotate_3dp(struct point3d_sint p3d_rot, struct point3d_sint *p3d_new, 
					sint angle_x, sint angle_y, sint angle_z, sint viewDist, sint viewSize)
{		   	
	// x-rotation
	p3d_new->y = ((p3d_rot.y*get_cos(angle_x))-(p3d_rot.z*get_sin(angle_x)))/100;
	p3d_new->z = ((p3d_rot.y*get_sin(angle_x))+(p3d_rot.z*get_cos(angle_x)))/100;

	// y-rotation
	p3d_new->x = ((p3d_rot.x*get_cos(angle_y))+(p3d_new->z*get_sin(angle_y)))/100;
	p3d_new->z = ((-p3d_rot.x*get_sin(angle_y))+(p3d_new->z*get_cos(angle_y)))/100;

	p3d_rot.x=p3d_new->x;	//backup x and y for z-rotation, abusing p3d_rot
	p3d_rot.y=p3d_new->y;

	// z-rotation
	p3d_new->x = ((p3d_rot.x*get_cos(angle_z))-(p3d_rot.y*get_sin(angle_z)))/100;
	p3d_new->y = ((p3d_rot.y*get_cos(angle_z))+(p3d_rot.x*get_sin(angle_z)))/100;
    
    // perspective (higher value = smaller)
    if(viewDist>0||viewSize>0)
    {
		p3d_new->x=(p3d_new->x*viewDist)/(p3d_new->z+viewSize);
		p3d_new->y=(p3d_new->y*viewDist)/(p3d_new->z+viewSize);
    }
}

void draw_string_sin(sint x, sint y, const uchar *str, sint x_s, uchar div, 
						uint f_color, uint b_color, uchar sx, uchar sy)
{
	while(*str!=0&&x<lcd_width)
	{
		if(x>=-(sint)((f_width-1)*(sx+1)))	// part of char inside display
			draw_char(x,y+get_sin(x_s)/div,*str,f_color,b_color,sx,sy);
		*str++;
		x+=f_width+space_char;
		x_s+=f_width+space_char;
	}
}

void draw_string(sint x, sint y, const uchar *str, uint f_color, uint b_color, uchar sx, uchar sy)
{	
    //f_color is text color
    //b_color is background color

    //sx is scale_x. Anything above 0 will scale upwards
    //sy is scale_y. Anything above 0 will scale upwards

	while(*str!=0&&x<lcd_width)
	{
		if(x>=-(sint)((f_width-1)*(sx+1)))	// part of char inside display
		{
			draw_char(x,y,*str,f_color,b_color,sx,sy);
			if(b_color!=TRANSP)
			{
				bit_num=space_char;
				while(bit_num--);	// draw a horizontal area (rect) in b_color
					fill_rect(x-space_char,x-1,y,y+(f_height*(sy+1))-1,b_color); 
			}
		}
		*str++;
		x+=(f_width+space_char);
	}
}

void draw_char_text_mode(sint x, sint y, uchar character, uint f_color, uint b_color, uchar sx, uchar sy)
{
	// need correction here
	x*=(f_width*(sx+1)+space_char);
	y*=(f_height*(sy+1));
	draw_bmap_8bit_v(x,y,ascii_table[character],f_width,f_height,f_color,b_color,sx,sy);
}

void draw_char(sint x, sint y, uchar character, uint f_color, uint b_color, uchar sx, uchar sy)
{
	draw_bmap_8bit_v(x,y,ascii_table[character],f_width,f_height,f_color,b_color,sx,sy);
}

void draw_bmap_8bit_v(sint x, sint y, const uchar *bmap, uint width, uint height, 
								uint f_color, uint b_color, uchar sx, uchar sy)
{
	sint x_off=0,y_off=0,x_index=0;
	bit_num=0,byte=0,w_index=0,h_index=0;
	if(x>=lcd_width||y>=lcd_height) return;
	if(y+(sint)height<0) return;
	if(x+(sint)width<0) return;
	if(x<0) 
	{
		w_index=x*-1;	// index in bitmap
		x_off=w_index;	// x offset in bitmap
		x=0;
	}
	else x*=(sx+1);		// move size of pixel-size (correction) -> CHANGE!
	byte=height;		// save real height
	if(height%8) height+=8;
	height/=8;
	if(y<0)
	{
		y*=-1;
		y_off=y/8;					
		bit_num=y%8;
		height-=y_off;
		y=0;
	}
	else y*=(sy+1);		// move size of pixel-size
	
	while(h_index<height&&y<lcd_height)
	{
		// vertical bit tester
		while(bit_num<8)
		{
			// horizontal pixel writing
			while(w_index<width&&(x+x_index)<lcd_width)
			{
				// if bit is "1" write a pixel in foreground color
				if(bmap[w_index+((h_index+y_off)*width)]&(1<<bit_num))
					draw_pixel_xy(x+x_index,y,f_color,sx,sy);
				// if bit is "0" write a pixel in background color
				else if(b_color!=TRANSP)
					draw_pixel_xy(x+x_index,y,b_color,sx,sy);

				w_index++;
				x_index+=sx+1;
			}
			w_index=x_off;
			x_index=0;
			bit_num++;
			y+=sy+1;
		}
		h_index++;
		w_index=x_off;
		bit_num=0;
	}
}

// modified bresenham algorithm with zoom factor
void draw_circle(sint xm, sint ym, sint r, uint color, uchar sx, uchar sy)
{
	sint x = -r, y = 0, err = 2-2*r; // II. Quadrant  
	do 
	{
		draw_pixel_xy(xm-x*(sx+1),ym+y*(sy+1),color,sx,sy); //   I. Quadrant 
		draw_pixel_xy(xm-y*(sy+1),ym-x*(sx+1),color,sx,sy); //  II. Quadrant 
		draw_pixel_xy(xm+x*(sx+1),ym-y*(sy+1),color,sx,sy); // III. Quadrant 
		draw_pixel_xy(xm+y*(sy+1),ym+x*(sx+1),color,sx,sy); //  IV. Quadrant 
		r = err;
		if (r <= y)
		{ 
			y++;  
			err += (y*2+1);
		}   // e_xy+e_y < 0 
		if (r > x || err > y) 
		{ 
			x++;
			err += (x*2+1); 
		} // e_xy+e_x > 0 or no 2nd y-step 
	} 
	while (x < 0);
}

// modified bresenham algorithm with zoom factor
void draw_line(sint x0, sint y0, sint x1, sint y1, uint color, uchar sx_s, uchar sy_s)
{
	sint dx=get_abs(x1-x0),		sx=x0<x1?1:-1;
   	sint dy=-get_abs(y1-y0), 	sy=y0<y1?1:-1; 
   	sint err=dx+dy, e2; // error value e_xy
 	for(;;)
	{
		draw_pixel_xy(x0*(sx_s+1),y0*(sy_s+1),color,sx_s,sy_s);
    	if (x0==x1 && y0==y1) break;
      	e2=2*err;
      	if(e2>=dy){err+=dy; x0+=sx;} // e_xy+e_x > 0
      	if(e2<=dx){err+=dx; y0+=sy;} // e_xy+e_y < 0
	}
}

void draw_rect(sint x0, sint x1, sint y0, sint y1, uint color, uchar sx, uchar sy)
{

    if(y0>=lcd_height||x0>=lcd_width) return;   // overflow
    if((x0+x1)<0||(y0+y1)<0) return;            // outside display
    if(x0<0) x0=0;
    if(y0<0) y0=0;
    if(x1>=lcd_width) x1=lcd_width;
    if(y1>=lcd_height) y1=lcd_height;

    draw_line(x0, y0, x1, y0, color, sx, sy);
    draw_line(x1, y0, x1, y1, color, sx, sy);
    draw_line(x1, y1, x0, y1, color, sx, sy);
    draw_line(x0, y1, x0, y0, color, sx, sy);
}
void fill_rect(sint x0, sint x1, sint y0, sint y1, uint color)
{
	// swap check maybe
	if(y0>=lcd_height||x0>=lcd_width) return;	// overflow
	if((x0+x1)<0||(y0+y1)<0) return;			// outside display
	if(x0<0) x0=0;						
	if(y0<0) y0=0;
	if(x1>=lcd_width) x1=lcd_width;	
	if(y1>=lcd_height) y1=lcd_height;
	uint x_start=x0;
	while(y0<=y1)
	{
		set_cursor(x0,y0++);
		while(x0++<=x1)
			draw_pixel(color);
		x0=x_start;
	}
} 

void draw_pixel_xy(sint x, sint y, uint color, uchar sx, uchar sy)
{
	// sx=1 and sy=1 => 1*1px = 2*2*px (4px) and so on
	if(sx||sy)
		fill_rect(x,x+sx,y,y+sy,color);
	else 
	{
		if(set_cursor(x,y))
			draw_pixel(color);
	}
}

// convert wavelength to RGB
// http://www.efg2.com/Lab/ScienceAndEngineering/Spectra.htm
uint wavelen2rgb(uint wavelen)
{
	uchar R,G,B;
	// normalize, 380nm-780nm
	if(wavelen<380) wavelen+=380;
	if(wavelen>780) wavelen=wavelen%780+380;
	if(wavelen>=380&&wavelen<440)
	{
		R=((-(wavelen-440))*R_MAX)/60;
		G=0;B=B_MAX;
	}
	else if(wavelen>=440&&wavelen<490)
	{
		G=((wavelen-440)*G_MAX)/50;
		R=0;B=B_MAX;
	}
	else if(wavelen>=490&&wavelen<510)
	{
		B=((-(wavelen-510))*B_MAX)/20;
		R=0;G=G_MAX;
	}
	else if(wavelen>=510&&wavelen<580)
	{
		R=((wavelen-510)*R_MAX)/70;
		G=G_MAX;B=0;
	}
	else if(wavelen>=580&&wavelen<645)
	{
		G=((-(wavelen-645))*G_MAX)/65;
		R=R_MAX;B=0;
	}
	else if(wavelen>=645&&wavelen<781)
	{
		R=R_MAX;
		G=0;B=0;
	}
	else
	{
		R=0;G=0;B=0;
	}
	return set_RGB16(R,G,B);
}

uint set_RGB16(uchar R, uchar G, uchar B)
{
	return (uint)(R<<11)+(uint)(G<<5)+(uint)B;
}

void draw_pixel(uint color)
{
	set_instruction(1,color>>8);
	set_instruction(1,color&0xFF);	
}

uchar check_xy(sint x, sint y)
{
	if(x<0||x>=lcd_width||y<0||y>=lcd_height) return 0;
	return 1;
}

void fill_display(uint width, uint height, uint color)
{
	set_cursor(0,0);
	while(height--)
	{
		while(width--)
			draw_pixel(color);
		width=lcd_width;
	}
}

uchar set_cursor(sint x, sint y)
{
	if(!set_cursor_x(x)||!set_cursor_y(y)) return 0;
	return 1;
}

uchar set_cursor_x(sint x)
{
	if(x<0||x>=lcd_width) return 0;
	set_instruction(0,0x2A);
	set_instruction(1,x>>8);
	set_instruction(1,x&0xFF);
	MEM_WRITE;
	return 1;
}

uchar set_cursor_y(sint y)
{
	if(y<0||y>=lcd_height) return 0;
	set_instruction(0,0x2B);
	set_instruction(1,y>>8);
	set_instruction(1,y&0xFF);
	MEM_WRITE;
	return 1;
}

void set_instruction(uchar register_sel, uchar number)
{
	if(register_sel) 
		P1OUT|=DC;				// data
	else P1OUT&=~DC;			// command
	
	P1OUT&=~CS;					// start condition. Set SS to low for the LCD to indicate data transfer.
	P2OUT|=CS2;
	while(IFG2 & UCB0TXIFG);		// TX buffer ready?
  	UCB0TXBUF=number;			// start transmission
}


uchar pollTouchScreen(uchar touchCommand){
    //send the command to touch
    uchar receivedChar=0;

    //set SS to low for the touchscreen controller
    P2OUT &= ~CS2;
    P1OUT |= CS;

    while (IFG2 & UCB0TXIFG); // USCI_B0 TX buffer ready?
    UCB0TXBUF = touchCommand; // Send signal to receive Y position

    P2OUT &= ~CS2;
    //then wait for buffer
    while (IFG2 & UCB0RXIFG); // USCI_B0 RX Received?
    receivedChar = UCB0RXBUF; // Store received data

    P2OUT |= CS2;
    P1OUT |= CS;
    //now interpret received data
    return receivedChar;
}


