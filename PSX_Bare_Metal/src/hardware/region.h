#ifndef REGION_H
#define REGION_H

#define REGION_PAL  1
#define REGION_NTSC 2

#ifndef REGION
#define REGION REGION_PAL
#endif

#if REGION == REGION_PAL

#define REGION_VIDEO_MODE   GPU_PAL
#define SCREEN_H_MODE       GPU_H320
#define SCREEN_V_MODE       GPU_V240
#define TARGET_FPS          50
#define SCREEN_W            320
#define SCREEN_H            256
#define DISPLAY_CENTER_Y    0xA3
#define DISPLAY_START_X     608

#elif REGION == REGION_NTSC

#define REGION_VIDEO_MODE   GPU_NTSC
#define SCREEN_H_MODE       GPU_H320
#define SCREEN_V_MODE       GPU_V240
#define TARGET_FPS          60
#define SCREEN_W            320
#define SCREEN_H            240
#define DISPLAY_CENTER_Y    0x88
#define DISPLAY_START_X     608

#else
#error "REGION must be REGION_PAL or REGION_NTSC"
#endif

#endif