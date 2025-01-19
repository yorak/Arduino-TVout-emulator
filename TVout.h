/*
Copyright (c) 2010 Myles Metzer
SDL2 version Copyright (c) 2025 Jussi Rasku

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/
/*
This library provides a simple method for outputting data to a SDL framebuffer.
It is meant for drop in replacement for the Arduino tvout.h when developing on
a computer.
*/
#ifndef SDL_TVOUT_H
#define SDL_TVOUT_H

#include <stdint.h>

// dymmy defines to make this appear to be Android
#define PROGMEM
#define PI 3.14159265358979323846
typedef uint8_t byte;

// macros for readability when selecting mode.
#define PAL						1
#define	NTSC					0
#define _PAL					1
#define _NTSC					0

#define WHITE					1
#define BLACK					0
#define INVERT					2

#define UP						0
#define DOWN					1
#define LEFT					2
#define RIGHT					3

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2
#define BYTE 0

// Macros for clearer usage
#define clear_screen()				fill(0)
#define invert(color)				fill(2)

/*
TVout.cpp contains a brief expenation of each function.
*/
class TVout {

public:
	// There is only one screen
	static uint8_t * screen;
	
	char begin(uint8_t mode);
	char begin(uint8_t mode, uint8_t x, uint8_t y);
	void end();
	
	//accessor functions
	unsigned char hres();
	unsigned char vres();
	char char_line();
	
	//flow control functions
	void delay(unsigned int x);
	void delay_frame(unsigned int x);
	unsigned long millis();
	
	//override setup functions
	void force_vscale(char sfactor);
	void force_outstart(uint8_t time);
	void force_linestart(uint8_t line);
	
	//basic rendering functions
	void set_pixel(uint8_t x, uint8_t y, char c);
	unsigned char get_pixel(uint8_t x, uint8_t y);
	void fill(uint8_t color);
	void shift(uint8_t distance, uint8_t direction);
	void draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, char c);
	void draw_row(uint8_t line, uint16_t x0, uint16_t x1, uint8_t c);
	void draw_column(uint8_t row, uint16_t y0, uint16_t y1, uint8_t c);
	void draw_rect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, char c, char fc = -1); 
	void draw_circle(uint8_t x0, uint8_t y0, uint8_t radius, char c, char fc = -1);
	void bitmap(uint8_t x, uint8_t y, const unsigned char * bmp, uint16_t i = 0, uint8_t width = 0, uint8_t lines = 0);

    //placeholders for printing 
    void println(const char[]);

	// tone functions
    void tone(unsigned int frequency, unsigned long durationMS);
    void tone(unsigned int frequency);
    void noTone();

private:
	uint8_t cursor_x,cursor_y;
	const unsigned char * font;
	
	void inc_txtline();
    void printNumber(unsigned long, uint8_t);
    void printFloat(double, uint8_t);
};

inline void sp(unsigned char x, unsigned char y, char c); 
#endif //SDL_TVOUT_H