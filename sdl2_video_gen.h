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

#ifndef SDL2_VIDEO_GEN_H
#define SDL2_VIDEO_GEN_H

typedef struct {
	volatile int scanLine;
	volatile unsigned long frames;
	unsigned char start_render;
	int lines_frame;	  	
	uint8_t vres;
	uint8_t hres_bytes;
	uint8_t output_delay; 	
	char vscale_const;		
	char vscale;			
	char vsync_end;			
	uint8_t * screen;
} TVout_vid;

extern volatile TVout_vid display;

extern void (*hbi_hook)();
extern void (*vbi_hook)();

char render_setup(uint8_t mode, uint8_t x, uint8_t y, uint8_t *scrnptr);

// Custom for SDL2
void render_end();
bool is_render_active();
char update();
void wait_simulated_vblank(); 

//tone generation properties
extern volatile long remainingToneVsyncs;



#endif // SDL2_VIDEO_GEN

