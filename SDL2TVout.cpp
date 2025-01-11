/*
 Original TVout Copyright (c) 2010 Myles Metzer
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

/* A note about how Color is defined for this version of TVout
 *
 * Where ever choosing a color is mentioned the following are true:
 * 	BLACK	=0
 *	WHITE	=1
 *	INVERT	=2
 *	All others will be ignored.
*/

#include "TVout.h"

// SDL2 specific includes
#include <SDL2/SDL.h>
#include <chrono>
#include <thread>


// TVout variables
uint8_t cursor_x = 0;
uint8_t cursor_y = 0;


/* Call this to start video output with the default resolution.
 * 
 * Arguments:
 *	mode:
 *		The video standard to follow:
 *		PAL		=1	=_PAL
 *		NTSC	=0	=_NTSC
 *
 * Returns:
 *	0 if no error.
 *	4 if there is not enough memory.
 */
char TVout::begin(uint8_t mode) {

    return begin(mode,128,96);
} // end of begin

char render_and_fb_setup(uint8_t mode, uint8_t x, uint8_t y) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return -1; // Initialization failed
    }

    // No specific vertical resolution limits for NTSC here, allowing flexibility.
    screen_width = x;
    screen_height = y;
    
    // Set default SDL window dimensions based on mode
    int window_width = (mode == NTSC ? 640 : 768) * mode_scaler;
    int window_height = (mode == NTSC ? 480 : 576) * mode_scaler;

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
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, screen_width, screen_height);
    if (!texture) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return -1;
    }

    // Allocate pixel buffer
    pixels = new uint32_t[screen_width * screen_height];
    if (!pixels) {
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return 4; // Memory allocation failure
    }

    // Start timer
    start_time = std::chrono::steady_clock::now();

    // Clear screen
    fill(BLACK);
    return 0;
}


/* call this to start video output with a specified resolution.
 *
 * Arguments:
 *	mode:
 *		The video standard to follow:
 *		PAL		=1	=_PAL
 *		NTSC	=0	=_NTSC
 *	x:
 *		Horizontal resolution must be divisable by 8.
 *	y:
 *		Vertical resolution.
 *
 *	Returns:
 *		0 if no error.
 *		1 if x is not divisable by 8.
 *		2 if y is to large (NTSC only cannot fill PAL vertical resolution by 8bit limit)
 *		4 if there is not enough memory for the frame buffer.
 */
char TVout::begin(uint8_t mode, uint8_t x, uint8_t y) {
    // check if x is divisable by 8
    if ( !(x & 0xF8))
        return 1;
    x = x/8;
    
    cursor_x = 0;
    cursor_y = 0;
    
    char setup_res = render_and_fb_setup(mode,x,y,screen);
    if (setup_res != 0) {
	return setup_res;
    }
    clear_screen();
    return 0;
} // end of begin

/* Stop video render and free the used memory.
 */
void TVout::end() {
    delete[] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void TVout::set_mode_scaler(float scaler) {
    mode_scaler = scaler;
}

/* Fill the screen with some color.
 * However, if color is INVERT the colors of the pixels in the buffer are inverted.
 * 
 * Arguments:
 *	color:
 *		The color to fill the screen with.
 *		(see color note at the top of this file)
*/
void TVout::fill(uint8_t color) {
    if (color == INVERT) {
        for (int i = 0; i < screen_width * screen_height; ++i) {
            pixels[i] = (pixels[i] == 0xFFFFFFFF) ? 0xFF000000 : 0xFFFFFFFF;
        }
    } else {
        uint32_t fill_color = (color == WHITE) ? 0xFFFFFFFF : 0xFF000000; // White or Black
        for (int i = 0; i < screen_width * screen_height; ++i) {
            pixels[i] = fill_color;
        }
    }
    SDL_UpdateTexture(texture, nullptr, pixels, screen_width * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

/* Set the color of a pixel
 * 
 * Arguments:
 *	x:
 *		The x coordinate of the pixel.
 *	y:
 *		The y coordinate of the pixel.
 *	c:
 *		The color of the pixel
 *		(see color note at the top of this file)
 */
void TVout::set_pixel(uint8_t x, uint8_t y, char c) {
    if (x >= screen_width || y >= screen_height) {
        return;
    }
    uint32_t color = (c == WHITE) ? 0xFFFFFFFF : 0xFF000000;
    pixels[y * screen_width + x] = color;
    SDL_UpdateTexture(texture, nullptr, pixels, screen_width * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

/* get the color of the pixel at x,y
 * 
 * Arguments:
 *	x:
 *		The x coordinate of the pixel.
 *	y:
 *		The y coordinate of the pixel.
 *
 * Returns:
 *	The color of the pixel.
 *	(see color note at the top of this file)
 *
 * Thank you gijs on the arduino.cc forum for the non obviouse fix.
*/
unsigned char TVout::get_pixel(uint8_t x, uint8_t y) {
    if (x >= screen_width || y >= screen_height) {
        return 0; // Out of bounds
    }
    return (pixels[y * screen_width + x] == 0xFFFFFFFF) ? WHITE : BLACK;
}

/* Gets the Horizontal resolution of the screen
 *
 * Returns: 
 *	The horizontal resolution.
*/
unsigned char TVout::hres() {
    return screen_width;
}

/* Gets the Vertical resolution of the screen
 *
 * Returns:
 *	The vertical resolution
*/
unsigned char TVout::vres() {
    return screen_height;
}

/* delay for x ms
 * The resolution is simulated with 16ms for NTSC and 20ms for PAL
 *
 * Arguments:
 *	x:
 *		The number of ms this function should consume.
*/
void TVout::delay(unsigned int x) {
    std::this_thread::sleep_for(std::chrono::milliseconds(x));
}

/* Delay for x frames, exits at the end of the last display line.
 * delay_frame(1) is useful prior to drawing so there is little/no flicker.
 *
 * Arguments:
 *	x:
 *		The number of frames to delay for.
 */
void TVout::delay_frame(unsigned int x) {
    unsigned int frame_time = (screen_height == 480) ? 16 : 20; // NTSC or PAL frame time in ms
    std::this_thread::sleep_for(std::chrono::milliseconds(frame_time * x));
}

/* Get the time in ms since begin was called.
 * The resolution is simulated with 16ms for NTSC and 20ms for PAL
 *
 * Returns:
 *	The time in ms since video generation has started.
*/
unsigned long TVout::millis() {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
}

/* force the number of times to display each line.
 *
 * Arguments:
 *	sfactor:
 *		The scale number of times to repeate each line.
 */
void TVout::force_vscale(char sfactor) {
    // This operation doesn't apply directly in SDL2.
}

/* force the output start time of a scanline in micro seconds.
 *
 * Arguments:
 *	time:
 *		The new output start time in micro seconds.
 */
void TVout::force_outstart(uint8_t time) {
    // This operation doesn't apply directly in SDL2.
}

/* force the start line for active video
 *
 * Arguments:
 *	line:
 *		The new active video output start line
 */
void TVout::force_linestart(uint8_t line) {
    // This operation doesn't apply directly in SDL2.
}
