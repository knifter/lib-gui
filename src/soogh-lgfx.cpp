#include "soogh-lgfx.h"

#include "soogh-conf.h"

LGFX                    _lgfx;
lv_disp_draw_buf_t  	_lv_draw_buf;
lv_color_t 			    _lv_color_buf1[LV_BUF_SIZE];
#ifdef GUI_DOUBLEBUF
lv_color_t 			    _lv_color_buf2[LV_BUF_SIZE];
#endif
lv_disp_drv_t 		    _lv_display_drv;        /*Descriptor of a display driver*/

#ifdef GUI_TOUCH
    lv_indev_drv_t 		_lv_touch_drv;           /*Descriptor of a input device driver*/
#endif

#ifdef GUI_KEYPAD
    uint32_t            _lvgl_key = 0;
    lv_indev_drv_t 		_lv_keys_drv;           /*Descriptor of a input device driver*/
	lv_indev_t*			_indev_keypad;
#endif

void lgfx_init()
{
    _lgfx.init();
    _lgfx.setRotation(1);
    _lgfx.setColorDepth(24);
};

void lvgl_init()
{
    lv_init();

#ifdef GUI_DOUBLEBUF
    lv_disp_draw_buf_init(&_lv_draw_buf, _lv_color_buf1, _lv_color_buf2, LV_BUF_SIZE);
#else
    lv_disp_draw_buf_init(&_lv_draw_buf, _lv_color_buf1, NULL, LV_BUF_SIZE);
#endif
    lv_disp_drv_init(&_lv_display_drv);          /*Basic initialization*/
    _lv_display_drv.flush_cb = lv_disp_cb;    /*Set your driver function*/
    _lv_display_drv.draw_buf = &_lv_draw_buf;        /*Assign the buffer to the display*/
    _lv_display_drv.hor_res = DISPLAY_WIDTH;   /*Set the horizontal resolution of the display*/
    _lv_display_drv.ver_res = DISPLAY_HEIGHT;   /*Set the vertical resolution of the display*/
    lv_disp_drv_register(&_lv_display_drv);      /*Finally register the driver*/

#ifdef GUI_TOUCH
    lv_indev_drv_init(&_lv_touch_drv);             /*Basic initialization*/
    _lv_touch_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
    _lv_touch_drv.read_cb = lv_touchpad_cb;      /*Set your driver function*/
    lv_indev_drv_register(&_lv_touch_drv);         /*Finally register the driver*/
#endif // GUI_TOUCH

#ifdef GUI_KEYPAD
    lv_indev_drv_init(&_lv_keys_drv);             /*Basic initialization*/
    _lv_keys_drv.type = LV_INDEV_TYPE_KEYPAD;    /*Touch pad is a pointer-like device*/
    _lv_keys_drv.read_cb = lv_keys_cb;      /*Set your driver function*/
    _indev_keypad = lv_indev_drv_register(&_lv_keys_drv);         /*Finally register the driver*/
    _lvgl_key = 0;
#endif // GUI_KEYPAD
};

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
    if(!_lgfx.getTouch( &touchX, &touchY))
    {
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    };
    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = touchX;
    data->point.y = touchY;

    // Serial.printf("Touch = (%d, %d)\n", touchX, touchY);
};
#endif

#ifdef GUI_KEYPAD
void lv_keys_cb(lv_indev_drv_t * indev, lv_indev_data_t * data)
{
    data->key = _lvgl_key;
    data->state = LV_INDEV_STATE_RELEASED;
    if(data->key)
    {
        // DBGUI_DBGG("LVGL KEY: %x", data->key);
        data->state = LV_INDEV_STATE_PRESSED;
    };
    _lvgl_key = 0;
	return;
};
#endif // GUI_KEYPAD
