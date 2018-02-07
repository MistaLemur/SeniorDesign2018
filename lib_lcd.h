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

#ifndef LIB_LCD_H_
#define LIB_LCD_H_

struct point3d_schar{schar x,y,z;};
struct point3d_sint{sint x,y,z;};

extern uchar space_char;			// space between chars

#define CS				0x0010		// chip select at P1.4
#define SDIN			0x0080		// data in at P1.7
#define SCLK			0x0020		// clk at P1.5
#define DC				0x0040		// Data/Cmd at P1.6
#define RES 			0x0001		// reset at P1.0

#define lcd_width		240
#define lcd_height		320	
				
#define TRANSP			C_FUCHSIA	// magic pink is transparency fake

#define DISPLAY_ON		set_instruction(0,0x29)
#define DISPLAY_OFF		set_instruction(0,0x28)
#define MEM_WRITE		set_instruction(0,0x2C)

#define R_MAX			31
#define G_MAX			63
#define B_MAX			31

// 16 VGA Colors (in RGB565, 16bit)
#define C_WHITE			0xFFFF
#define C_SILVER		0xC618
#define C_BLACK			0x0000
#define C_GREY			0x8410
#define C_RED			0xF800
#define C_MAROON		0x8000
#define C_FUCHSIA		0xF81F		
#define C_PURPLE		0x8010
#define C_LIME			0x07E0
#define C_GREEN			0x0400
#define C_YELLOW		0xFFE0
#define C_OLIVE			0x8400
#define C_BLUE			0x001F
#define C_NAVY			0x0010
#define C_AQUA			0x07FF
#define C_TEAL			0x0410

#define C_DARK_BLUE		0x0002


void rotate_3dp_array(const schar[][3],struct point3d_schar*,uchar,uchar end,sint,sint,sint,sint,sint);
void rotate_3dp(struct point3d_sint,struct point3d_sint*,sint,sint,sint,sint,sint);
void draw_3d_obj(sint,sint,struct point3d_schar*,const uchar[][2],uchar,uchar,uint);
void draw_string_sin(sint,sint,const uchar*,sint,uchar,uint,uint,uchar,uchar);
void draw_string(sint,sint,const uchar*,uint,uint,uchar,uchar);
void draw_char_text_mode(sint,sint,uchar,uint,uint,uchar,uchar);
void draw_char(sint,sint,uchar,uint,uint,uchar,uchar);
void draw_bmap_8bit_v(sint,sint,const uchar*,uint,uint,uint,uint,uchar,uchar);
void draw_circle(sint,sint,sint,uint,uchar,uchar);
void draw_line(sint,sint,sint,sint,uint,uchar,uchar);
void fill_rect(sint,sint,sint,sint,uint);
void draw_rect(sint x0, sint x1, sint y0, sint y1, uint color, uchar sx, uchar sy);
uchar check_xy(sint,sint);
void draw_pixel_xy(sint,sint,uint,uchar,uchar);
void draw_pixel(uint);
uint wavelen2rgb(uint);
uint set_RGB16(uchar,uchar,uchar);
void fill_display(uint,uint,uint);
uchar set_cursor(sint,sint);
uchar set_cursor_x(sint);
uchar set_cursor_y(sint);
void set_instruction(uchar,uchar);

#endif /*LIB_LCD_H_*/
