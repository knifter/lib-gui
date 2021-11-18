#include "soogh-lgfx.h"

#include "soogh-conf.h"

LGFX _lgfx;
lv_disp_draw_buf_t 	_lv_draw_buf;
lv_color_t 			_lv_color_buf[LV_BUF_SIZE];
lv_disp_drv_t 		_lv_display_drv;        /*Descriptor of a display driver*/

#ifdef GUI_TOUCH
    lv_indev_drv_t 		_lv_touch_drv;           /*Descriptor of a input device driver*/
#endif

#ifdef GUI_KEYPAD
    uint32_t _last_key = 0;
    lv_indev_drv_t 		_lv_keys_drv;           /*Descriptor of a input device driver*/
	lv_indev_t*			_indev_keypad;
#endif

void lv_disp_cb(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p)
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    _lgfx.startWrite();
    _lgfx.setAddrWindow( area->x1, area->y1, w, h );
    _lgfx.writePixelsDMA((lgfx::rgb565_t *)&color_p->full, w * h);
    _lgfx.endWrite();

    lv_disp_flush_ready( disp );
};

#ifdef GUI_TOUCH
void lv_touchpad_cb(lv_indev_drv_t * indev, lv_indev_data_t * data)
{
    uint16_t touchX, touchY;
    if(_lgfx.getTouch( &touchX, &touchY))
    {
        data->state = LV_INDEV_STATE_PR;

        /*Set the coordinates*/
        data->point.x = touchX;
        data->point.y = touchY;

		Serial.printf("Touch = (%d, %d)\n", touchX, touchY);
    };
    data->state = LV_INDEV_STATE_REL;
};
#endif

#ifdef GUI_KEYPAD
void lv_keys_cb(lv_indev_drv_t * indev, lv_indev_data_t * data)
{
    data->key = _last_key;
    data->state = LV_INDEV_STATE_RELEASED;
    if(data->key)
    {
        // DBGUI_DBGG("LVGL KEY: %x", data->key);
        data->state = LV_INDEV_STATE_PRESSED;
    };
    _last_key = 0;
	return;
};
#endif // GUI_KEYPAD
