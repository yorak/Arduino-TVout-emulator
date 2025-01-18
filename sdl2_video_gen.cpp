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



void empty() {}

int renderLine;
volatile TVout_vid display;
void (*render_line)();			
void (*line_handler)();		
void (*hbi_hook)() = &empty;
void (*vbi_hook)() = &empty;

// SDL variables
uint8_t render_video_mode = _PAL;
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Texture *texture = nullptr;
float mode_scaler = 1.5f;  // Default scaling factor
const double NTSC_FRAME_TIME = 1000.0 / 59.94;    // ~16.683ms
const double PAL_FRAME_TIME = 1000.0 / 50.0;     // 20ms exactly    
SDL_TimerID timer_id = -1;
SDL_atomic_t sdl_render_is_active;  
SDL_mutex* frame_mutex;
SDL_cond* frame_condition;

// sound properties
volatile long remainingToneVsyncs;

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


// Timer callback that triggers frame updates
Uint32 frame_timer_callback(Uint32 interval, void* /*param*/) {
    if (!SDL_AtomicGet(&sdl_render_is_active)) {
        return 0;  // Return 0 to stop the timer
    }

    SDL_LockMutex(frame_mutex);

    // Update texture and render exactly on the timer interval
    update_texture(texture, display.screen, display.hres, display.vres);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    display.frames++;
    SDL_CondSignal(frame_condition);

    SDL_UnlockMutex(frame_mutex);

    return interval;
}

char render_setup(uint8_t mode, uint8_t x, uint8_t y, uint8_t *scrnptr) {
    display.screen = scrnptr;
    display.hres = x;
    display.vres = y;
    display.frames = 0;
    render_video_mode = mode;
    
    if (mode) {
        display.vscale_const = _PAL_LINE_DISPLAY/display.vres - 1;
        display.start_render = _PAL_LINE_MID - ((display.vres * (display.vscale_const+1))/2);
        display.output_delay = _PAL_CYCLES_OUTPUT_START;
        display.vsync_end = _PAL_LINE_STOP_VSYNC;
        display.lines_frame = _PAL_LINE_FRAME;
    } else {
        display.vscale_const = _NTSC_LINE_DISPLAY/display.vres - 1;
        display.start_render = _NTSC_LINE_MID - ((display.vres * (display.vscale_const+1))/2) + 8;
        display.output_delay = _NTSC_CYCLES_OUTPUT_START;
        display.vsync_end = _NTSC_LINE_STOP_VSYNC;
        display.lines_frame = _NTSC_LINE_FRAME;
    }
    
    display.vscale = display.vscale_const;
    display.scanLine = display.lines_frame+1;
    
    // Set default SDL window dimensions based on mode
    int window_width = (mode == _NTSC ? 640 : 768) * mode_scaler;
    int window_height = (mode == _NTSC ? 480 : 576) * mode_scaler;

    // Initialize SDL without vsync
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow("TVout Simulation", 
                            SDL_WINDOWPOS_CENTERED, 
                            SDL_WINDOWPOS_CENTERED,
                            window_width, 
                            window_height, 
                            SDL_WINDOW_SHOWN);

    if (!window) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    
    // Create renderer without vsync flag
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    
    // Create texture for pixel manipulation
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, x, y);
    if (!texture) {
        fprintf(stderr, "Could not create texture! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Explicitly disable vsync
    SDL_GL_SetSwapInterval(0);

    // Make sure the SDL window update and the android emulation threads do
    //  not trash the shared data.
    frame_mutex = SDL_CreateMutex();
    frame_condition = SDL_CreateCond();
    SDL_AtomicSet(&sdl_render_is_active, 1);

    // Create high-resolution timer for precise timing of frame update
    timer_id = SDL_AddTimer(
        mode ? (Uint32)(1000.0/50.0) : (Uint32)(1000.0/59.94),
        frame_timer_callback,
        NULL
    );

    return 0;
}

/* Stop video render and free the used memory.
 */
void render_end() {
    // Setting this to 0 prevents screen update on any remaining timer callback.
    SDL_AtomicSet(&sdl_render_is_active, 0);
    SDL_RemoveTimer(timer_id);

    // Turn off rendering altogether.
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
}

char update() {
    if (!SDL_AtomicGet(&sdl_render_is_active)) {
        return 0; // Have to wait for the renderer to come online.
    }

    // Do not message pump while frame is being updated.
    SDL_LockMutex(frame_mutex);
    SDL_Event event;
    // Event pump
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                SDL_UnlockMutex(frame_mutex);
                render_end();
                return 1;
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_CLOSE:
                        SDL_UnlockMutex(frame_mutex);
                        render_end();
                        return 1;
                }
                break;
            default:
                // TODO: Handle other events (keyboard, mouse, etc) as needed, 
                //  and set the "Arduino" digital / analog pins accordingly.
                processSDLInputEvent(event);
                // break;
            
        }
    }
    SDL_UnlockMutex(frame_mutex);
    return 0;
}

void wait_simulated_vblank() {
    SDL_LockMutex(frame_mutex);
    SDL_CondWait(frame_condition, frame_mutex);  // Waits for the signal
    SDL_UnlockMutex(frame_mutex);
}
