// We need this to call update.
#include "sdl2_video_gen.h"
#include <cstdio>   

int main() {
    long loop_cnt = 0;
    setup();
    
    while(1) {
        // TODO: the screen should in fact run in a separate thread with the 
        //  sync from the loop with TV.delay_frame(1);
        if (update()!=0) break;
        loop();
    }
    return 0;
}