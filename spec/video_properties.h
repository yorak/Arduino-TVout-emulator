/* This File contains the timing definitions for the TVout AVR composite video
 generation Library
*/
#ifndef VIDEO_TIMING_H
#define VIDEO_TIMING_H

// To avoid circular dependency
#define _NTSC 0 
#define _PAL  1

// IDK what this should be on PC. 
#define F_CPU 8000000UL

#define _CYCLES_PER_US                  (F_CPU / 1000000)

#define _TIME_HORZ_SYNC                 4.7
#define _TIME_VIRT_SYNC                 58.85
#define _TIME_ACTIVE                    46
#define _CYCLES_VIRT_SYNC               ((_TIME_VIRT_SYNC * _CYCLES_PER_US) - 1)
#define _CYCLES_HORZ_SYNC               ((_TIME_HORZ_SYNC * _CYCLES_PER_US) - 1)

// Timing settings for NTSC
#define _NTSC_TIME_SCANLINE             63.55
#define _NTSC_TIME_OUTPUT_START         12

#define _NTSC_LINE_FRAME                262
#define _NTSC_LINE_START_VSYNC          0
#define _NTSC_LINE_STOP_VSYNC           3
#define _NTSC_LINE_DISPLAY              216
#define _NTSC_LINE_MID                  ((_NTSC_LINE_FRAME - _NTSC_LINE_DISPLAY) / 2 + _NTSC_LINE_DISPLAY / 2)

#define _NTSC_CYCLES_SCANLINE           ((_NTSC_TIME_SCANLINE * _CYCLES_PER_US) - 1)
#define _NTSC_CYCLES_OUTPUT_START       ((_NTSC_TIME_OUTPUT_START * _CYCLES_PER_US) - 1)

// Timing settings for PAL
#define _PAL_TIME_SCANLINE              64
#define _PAL_TIME_OUTPUT_START          12.5

#define _PAL_LINE_FRAME                 312
#define _PAL_LINE_START_VSYNC           0
#define _PAL_LINE_STOP_VSYNC            7
#define _PAL_LINE_DISPLAY               260
#define _PAL_LINE_MID                   ((_PAL_LINE_FRAME - _PAL_LINE_DISPLAY) / 2 + _PAL_LINE_DISPLAY / 2)

#define _PAL_CYCLES_SCANLINE            ((_PAL_TIME_SCANLINE * _CYCLES_PER_US) - 1)
#define _PAL_CYCLES_OUTPUT_START        ((_PAL_TIME_OUTPUT_START * _CYCLES_PER_US) - 1)

#endif
