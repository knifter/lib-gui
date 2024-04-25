#ifndef __SOOGH_LGFX_H
#define __SOOGH_LGFX_H

// We need to define these in this order
#include "soogh-conf.h"

#define LGFX_USE_V1
#define LGFX_AUTODETECT

#include <LovyanGFX.hpp>

#include <lvgl.h>

#ifdef SOOGH_DEV_M5CORE
    extern LGFX _lgfx;
#endif

#ifdef SOOGH_DEV_WT32SC01
    class LGFX_SC01 : public lgfx::LGFX_Device
    {
        lgfx::Panel_ST7796  _panel_instance;  // ST7796UI
        lgfx::Bus_Parallel8 _bus_instance;    // MCU8080 8B
        lgfx::Light_PWM     _light_instance;
    #ifdef SOOGH_TOUCH
        lgfx::Touch_FT5x06  _touch_instance;
    #endif
        public:
            LGFX_SC01(void);
    };
    extern LGFX_SC01 _lgfx;
#endif

#ifdef SOOGH_ENCODER_KEYS
    extern uint32_t             lvgl_enc_last_key;
    extern bool                 lvgl_enc_pressed;
	extern lv_indev_t*          lvgl_indev_keyenc;
#endif

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
