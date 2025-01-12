/*
 Copyright (c) 2025 Jussi Rasku

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

#include "sdl2_video_gen.h"
#include "spec/video_properties.h"

// SDL2 specific includes
#include <SDL2/SDL.h>
#include <chrono>

void empty() {}

int renderLine;
TVout_vid display;
void (*render_line)();			//remove me
void (*line_handler)();			//remove me
void (*hbi_hook)() = &empty;
void (*vbi_hook)() = &empty;

// SDL variables
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Texture *texture = nullptr;
float mode_scaler = 1.5f;  // Default scaling factor
std::chrono::time_point<std::chrono::steady_clock> start_time;

// sound properties
volatile long remainingToneVsyncs;


char render_setup(uint8_t mode, uint8_t x, uint8_t y, uint8_t *scrnptr) {
    display.screen = scrnptr;
	display.hres = x;
	display.vres = y;
	display.frames = 0;
	
	if (mode)
		display.vscale_const = _PAL_LINE_DISPLAY/display.vres - 1;
	else
		display.vscale_const = _NTSC_LINE_DISPLAY/display.vres - 1;
	display.vscale = display.vscale_const;


    if (mode) {
		display.start_render = _PAL_LINE_MID - ((display.vres * (display.vscale_const+1))/2);
		display.output_delay = _PAL_CYCLES_OUTPUT_START;
		display.vsync_end = _PAL_LINE_STOP_VSYNC;
		display.lines_frame = _PAL_LINE_FRAME;
	}
	else {
		display.start_render = _NTSC_LINE_MID - ((display.vres * (display.vscale_const+1))/2) + 8;
		display.output_delay = _NTSC_CYCLES_OUTPUT_START;
		display.vsync_end = _NTSC_LINE_STOP_VSYNC;
		display.lines_frame = _NTSC_LINE_FRAME;
	}

	display.scanLine = display.lines_frame+1;
	line_handler = &vsync_line;


    // Set default SDL window dimensions based on mode
    int window_width = (mode == _NTSC ? 640 : 768) * mode_scaler;
    int window_height = (mode == _NTSC ? 480 : 576) * mode_scaler;

     // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return -1; // Initialization failed
    }

    // Create SDL window and renderer
    window = SDL_CreateWindow("TVout Simulation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_SHOWN);
    if (!window) {
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        return -1;
    }

    // Create texture for pixel manipulation
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, x, y);
    if (!texture) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return -1;
    }

    // Start timer
    start_time = std::chrono::steady_clock::now();

    return 0;
}


/* Stop video render and free the used memory.
 */
void render_end() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

static void update_texture(SDL_Texture* texture, unsigned char* screen, int width, int height) {
    // Create a buffer for the 32-bit pixels
    uint32_t* pixels = (uint32_t*)malloc(width * height * sizeof(uint32_t));
    if (!pixels) {
        SDL_Log("Failed to allocate pixel buffer");
        return;
    }

    // Convert 1-bit pixels to 32-bit ARGB
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Calculate byte index and bit position
            int byte_index = (y * width + x) / 8;
            int bit_pos = 7 - ((y * width + x) % 8); // MSB first
            
            // Extract the bit value
            int bit = (screen[byte_index] >> bit_pos) & 1;
            
            // DEBUG
            printf("%c%s%s", bit ? '#' : ' ', (x == width-1) ? "\n" : "", (x == width-1 && y == height-1) ? "\n" : "");

            // Convert to white (0xFFFFFFFF) or black (0xFF000000)
            // Format: 0xAARRGGBB
            pixels[y * width + x] = bit ? 0xFFFFFFFF : 0xFF000000;
        }
    }

    // Update the texture
    void* texture_pixels;
    int pitch;
    if (SDL_LockTexture(texture, NULL, &texture_pixels, &pitch) < 0) {
        SDL_Log("Failed to lock texture: %s", SDL_GetError());
        free(pixels);
        return;
    }

    // Copy our pixel buffer to the texture
    memcpy(texture_pixels, pixels, width * height * sizeof(uint32_t));
    
    // Clean up
    SDL_UnlockTexture(texture);
    free(pixels);
}

char update() {
    SDL_Event event;
    // Event pump
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                return -1;
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_CLOSE:
                        return -1;
                }
                break;
            // TODO: Handle other events (keyboard, mouse, etc) as needed, 
            //  and set the "Arduino" digital / analog pins accordingly.
        }
    }

    // Copy the framebuffer to the texture and show it
    update_texture(texture, display.screen, display.hres, display.vres);
    SDL_RenderCopy(renderer, texture, NULL, NULL);  // Add this line
    SDL_RenderPresent(renderer);
    return 0;
}




void blank_line() {
		
	if ( display.scanLine == display.start_render) {
		renderLine = 0;
		display.vscale = display.vscale_const;
		line_handler = &active_line;
	}
	else if (display.scanLine == display.lines_frame) {
		line_handler = &vsync_line;
	}
	
	display.scanLine++;
}

void active_line() {
	wait_until(display.output_delay);
	render_line();
	if (!display.vscale) {
		display.vscale = display.vscale_const;
		renderLine += display.hres;
	}
	else
		display.vscale--;
		
	if ((display.scanLine + 1) == (int)(display.start_render + (display.vres*(display.vscale_const+1))))
		line_handler = &blank_line;
		
	display.scanLine++;
}

void vsync_line() {
	if (display.scanLine >= display.lines_frame) {
		display.scanLine = 0;
		display.frames++;

		if (remainingToneVsyncs != 0)
		{
			if (remainingToneVsyncs > 0)
			{
				remainingToneVsyncs--;
			}

		} else
		{
			//??
            //TCCR2B = 0; //stop the tone
 			//PORTB &= ~(_BV(SND_PIN));
		}

	}
	else if (display.scanLine == display.vsync_end) {
		line_handler = &blank_line;
	}
	display.scanLine++;
}


inline void wait_until(uint8_t /*time*/) {
    /*IDK where this would even be used?*/
}

void render_line6c() {
}

void render_line5c() {
}

void render_line4c() {
}

void render_line3c() {
}