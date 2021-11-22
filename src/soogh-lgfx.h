#ifndef __SOOGH_LGFX_H
#define __SOOGH_LGFX_H

// We need to define these in this order
#include "soogh-conf.h"

#define LGFX_USE_V1
#define LGFX_AUTODETECT
// #include <LGFX_TFT_eSPI.hpp>
#include <LovyanGFX.hpp>

#include <lvgl.h>

extern LGFX _lgfx;

void lgfx_init();
void lvgl_init();

void lv_disp_cb(lv_disp_drv_t*, const lv_area_t*, lv_color_t*);
extern lv_disp_draw_buf_t 	    _lv_draw_buf;
extern lv_color_t 			    _lv_color_buf1[LV_BUF_SIZE];
#ifdef GUI_DOUBLEBUF
    extern lv_color_t 			    _lv_color_buf2[LV_BUF_SIZE];
#endif
extern lv_disp_drv_t 		    _lv_display_drv;        /*Descriptor of a display driver*/

#ifdef GUI_TOUCH
    extern lv_indev_drv_t 		_lv_touch_drv;           /*Descriptor of a input device driver*/
    void lv_touchpad_cb(lv_indev_drv_t *, lv_indev_data_t *);
#endif

#ifdef GUI_KEYPAD
    extern lv_indev_drv_t 		_lv_keys_drv;           /*Descriptor of a input device driver*/
    void lv_keys_cb(lv_indev_drv_t *, lv_indev_data_t *);
	extern lv_indev_t*			_lv_indev_keypad;
#endif

#endif // __SOOGH_LGFX_H