#include "soogh-lgfx.h"

#include "soogh-conf.h"

// #define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lvgl.h>
#include "esp_heap_caps.h"

// Draw buffer(s)
#define LV_BUF_SIZE					(DISPLAY_HEIGHT*DISPLAY_WIDTH/10)
lv_disp_draw_buf_t  	_lv_draw_buf;
lv_color_t*             _lv_color_buf1 = nullptr; 
lv_color_t*             _lv_color_buf2 = nullptr;

// Display driver
lv_disp_drv_t 		    _lv_display_drv;        /*Descriptor of a display driver*/
static void lv_disp_cb(lv_disp_drv_t*, const lv_area_t*, lv_color_t*);

#ifdef SOOGH_TOUCH
    lv_indev_drv_t 		_lv_touch_drv;           /*Descriptor of a input device driver*/
    static void lv_touchpad_cb(lv_indev_drv_t *, lv_indev_data_t *);
#endif

// Keys2LVGL conversion
#ifdef SOOGH_ENCODER_KEYS
    uint32_t            lvgl_enc_last_key = 0;
    bool                lvgl_enc_pressed = false;
    lv_indev_drv_t 		_lv_keys_drv;           /*Descriptor of a input device driver*/
	lv_indev_t*			lvgl_indev_keyenc;
    static void lv_keys_cb(lv_indev_drv_t *, lv_indev_data_t *);
#endif

// Device Configs
#ifdef SOOGH_DEV_M5CORE
// M%Stack has Lovyan Auto-detect, apparently
LGFX _lgfx;
#endif

#ifdef SOOGH_DEV_WT32SC01
LGFX_SC01::LGFX_SC01(void)
{
    {
        auto cfg = _bus_instance.config();
        cfg.freq_write = 40000000;    
        cfg.pin_wr = 47;             
        cfg.pin_rd = -1;             
        cfg.pin_rs = 0;              

        // gfx data interface, 8bit MCU (8080)
        cfg.pin_d0 = 9;              
        cfg.pin_d1 = 46;             
        cfg.pin_d2 = 3;              
        cfg.pin_d3 = 8;              
        cfg.pin_d4 = 18;             
        cfg.pin_d5 = 17;             
        cfg.pin_d6 = 16;             
        cfg.pin_d7 = 15;             
        _bus_instance.config(cfg);   

        _panel_instance.setBus(&_bus_instance);      
    };

    {
        auto cfg = _panel_instance.config();    
        cfg.pin_cs           =    -1;  
        cfg.pin_rst          =    4;  
        cfg.pin_busy         =    -1; 

        // Switch W/H because we rotate the display
        #if SOOGH_DISP_ROTATE == 1
        cfg.panel_width      =   DISPLAY_HEIGHT;
        cfg.panel_height     =   DISPLAY_WIDTH;
        #else
        cfg.panel_width      =   DISPLAY_WIDTH;
        cfg.panel_height     =   DISPLAY_HEIGHT;
        #endif
        cfg.offset_x         =     0;
        cfg.offset_y         =     0;
        cfg.offset_rotation  =     0;
        cfg.dummy_read_pixel =     8;
        cfg.dummy_read_bits  =     1;
        cfg.readable         = false;
        cfg.invert           =  true;
        cfg.rgb_order        = false;
        cfg.dlen_16bit       = false;
        cfg.bus_shared       =  true;

        _panel_instance.config(cfg);
    };

    {
        auto cfg = _light_instance.config();    
        cfg.pin_bl = 45;              
        cfg.invert = false;           
        cfg.freq   = 44100;           
        cfg.pwm_channel = 7;          
        _light_instance.config(cfg);

        _panel_instance.setLight(&_light_instance);  
    };

#ifdef SOOGH_TOUCH
    { 
        auto cfg = _touch_instance.config();
        cfg.x_min      = 0;
        cfg.x_max      = 319;
        cfg.y_min      = 0;  
        cfg.y_max      = 479;
        cfg.pin_int    = 7;  
        cfg.bus_shared = true; 
        cfg.offset_rotation = 0;

        cfg.i2c_port = 1;
        cfg.i2c_addr = 0x38;
        cfg.pin_sda  = 6;   
        cfg.pin_scl  = 5;   
        cfg.freq = 400000;  
        _touch_instance.config(cfg);

        _panel_instance.setTouch(&_touch_instance);  
    };
#endif

    setPanel(&_panel_instance); 
};
LGFX_SC01 _lgfx;
#endif

void lgfx_init()
{
    _lgfx.init();
    _lgfx.setRotation(SOOGH_DISP_ROTATE);
#ifdef SOOGH_DEV_M5CORE
    _lgfx.setColorDepth(24);
#endif
};

#if LV_USE_LOG
void serial_log_cb(const char* line)
{
    Serial.print(line);
};
#endif

void lvgl_init()
{
    _lv_color_buf1 = (lv_color_t*) heap_caps_malloc(LV_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    if(!_lv_color_buf1)
    {
        // lv_color_t 			    _lv_color_buf1[LV_BUF_SIZE];
    };
    _lv_color_buf2 = nullptr;
#ifdef SOOGH_DOUBLEBUF
    _lv_color_buf2 = (lv_color_t*) heap_caps_malloc(LV_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    if(!_lv_color_buf2)
    {
        // lv_color_t 			    _lv_color_buf2[LV_BUF_SIZE];
    };
#endif

    lv_init();

#if LV_USE_LOG
    lv_log_register_print_cb(serial_log_cb);
#endif

    lv_disp_draw_buf_init(&_lv_draw_buf, _lv_color_buf1, _lv_color_buf2, LV_BUF_SIZE);

    lv_disp_drv_init(&_lv_display_drv);          /*Basic initialization*/
    _lv_display_drv.flush_cb = lv_disp_cb;    /*Set your driver function*/
    _lv_display_drv.draw_buf = &_lv_draw_buf;        /*Assign the buffer to the display*/
    _lv_display_drv.hor_res = DISPLAY_WIDTH;   /*Set the horizontal resolution of the display*/
    _lv_display_drv.ver_res = DISPLAY_HEIGHT;   /*Set the vertical resolution of the display*/
    lv_disp_drv_register(&_lv_display_drv);      /*Finally register the driver*/

#ifdef SOOGH_TOUCH
    lv_indev_drv_init(&_lv_touch_drv);             /*Basic initialization*/
    _lv_touch_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
    _lv_touch_drv.read_cb = lv_touchpad_cb;      /*Set your driver function*/
    lv_indev_drv_register(&_lv_touch_drv);         /*Finally register the driver*/
#endif // GUI_TOUCH

#ifdef SOOGH_ENCODER_KEYS
    lv_indev_drv_init(&_lv_keys_drv);             /*Basic initialization*/
    _lv_keys_drv.type = LV_INDEV_TYPE_ENCODER;    
    _lv_keys_drv.read_cb = lv_keys_cb;      /*Set your driver function*/
    lvgl_indev_keyenc = lv_indev_drv_register(&_lv_keys_drv);         /*Finally register the driver*/
    lvgl_enc_last_key = 0;
    lvgl_enc_pressed = false;
#endif // SOOGH_ENCODER_KEYS
};

static void lv_disp_cb(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p)
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    _lgfx.startWrite();
#ifdef SOOGH_DEV_M5CORE
    // _lgfx.setAddrWindow( area->x1, area->y1, w, h );
    // _lgfx.writePixelsDMA((lgfx::rgb565_t *)&color_p->full, w * h);
    _lgfx.pushImageDMA(area->x1, area->y1, w, h, (lgfx::rgb565_t *)&color_p->full);
#endif
#ifdef SOOGH_DEV_WT32SC01
    //_lgfx.setAddrWindow( area->x1, area->y1, w, h );
    //_lgfx.writePixelsDMA((lgfx::swap565_t *)&color_p->full, w * h);
    //_lgfx.pushImageDMA(area->x1, area->y1, w, h, (lgfx::swap565_t *)&color_p->full);
#endif
    _lgfx.endWrite();

    lv_disp_flush_ready( disp );
};

#ifdef SOOGH_TOUCH
static void lv_touchpad_cb(lv_indev_drv_t * indev, lv_indev_data_t * data)
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

#ifdef SOOGH_ENCODER_KEYS
void lv_keys_cb(lv_indev_drv_t * indev, lv_indev_data_t * data)
{
    data->key = lvgl_enc_last_key;            /*Get the last pressed or released key*/

    if(lvgl_enc_pressed) 
        data->state = LV_INDEV_STATE_PRESSED;
    else
        data->state = LV_INDEV_STATE_RELEASED;
};
#endif // SOOGH_ENCODER_KEYS
