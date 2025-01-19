// We need this to call update.
#include <cstdio>
#include <thread>

#include "sdl2_video_gen.h"
#include "TVout.h"

int main() {
    bool running = true;
    std::thread arduino_thread([&running]() {
        setup();
        while (running) {
            loop();
            SDL_Delay(1);  // Prevent tight loop
        }
    });

    while (running) {
        if (update() != 0) {
            running = false;
        }
        SDL_Delay(1);  // Prevent tight loop
    }

    // Clean shutdown
    TVout justInCaseOneWasCreated; 
    justInCaseOneWasCreated.end();
    if (arduino_thread.joinable()) {
        arduino_thread.join();
    }
    render_end();
    return 0;
}
