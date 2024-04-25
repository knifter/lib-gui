#ifndef __SOOGH_CONF_H
#define __SOOGH_CONF_H

// #define SOOGH_TOUCH
// #define SOOGH_DOUBLEBUF // NEEDS TESTINGS!
// #define SOOGH_KEYPAD
// #define SOOGH_DEV_M5CORE
// #define SOOGH_DEV_SC01

// #define BOOTSCREEN_TIMEOUT_MS		5000
#ifdef SOOGH_DEV_M5CORE
	#define DISPLAY_HEIGHT				240
	#define DISPLAY_WIDTH				320
    #define SOOGH_DISP_ROTATE           1
    #define SOOGH_ENCODER_KEYS          1
#endif

#ifdef SOOGH_DEV_WT32SC01
    #define SOOGH_TOUCH
    #define SOOGH_DOUBLEBUF
    #define SOOGH_DISP_ROTATE           1
    #define DISPLAY_HEIGHT				320
    #define DISPLAY_WIDTH				480
#endif

#endif // __SOOGH_CONF_H
