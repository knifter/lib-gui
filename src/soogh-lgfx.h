#ifndef __SOOGH_LGFX_H
#define __SOOGH_LGFX_H

// We need to define these in this order
#include "soogh-conf.h"

#define LGFX_USE_V1
#define LGFX_AUTODETECT

#include <LovyanGFX.hpp>

#include <lvgl.h>

#ifdef SOOGH_ENCODER_KEYS
    extern uint32_t             lvgl_enc_last_key;
    extern bool                 lvgl_enc_pressed;
	extern lv_indev_t*          lvgl_indev_keyenc;
#endif
       
extern LGFX _lgfx;

void lgfx_init();
void lvgl_init();

#ifdef SOOGH_TOUCH
    extern lv_indev_drv_t 		_lv_touch_drv;           /*Descriptor of a input device driver*/
#endif

#ifdef SOOGH_ENCODER_KEYS
	extern lv_indev_drv_t 		_lv_keys_drv;           /*Descriptor of a input device driver*/
	extern lv_indev_t*			_lv_indev_keypad;
#endif

#endif // __SOOGH_LGFX_H
