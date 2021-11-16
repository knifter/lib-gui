#include "soogh-gui.h"

#include <lvgl.h>
#include <tools-log.h>

#include "soogh-conf.h"
#include "soogh-screen.h"

#define LGFX_USE_V1
#define LGFX_AUTODETECT
// #define LGFX_M5STACK_CORE2         // M5Stack Core2
#include <LGFX_TFT_eSPI.hpp>

void lv_disp_cb(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p);
#ifdef GUI_TOUCH
static lv_indev_drv_t 		_lv_touch_drv;           /*Descriptor of a input device driver*/
void lv_touchpad_cb(lv_indev_drv_t * indev, lv_indev_data_t * data);
#endif
#ifdef GUI_KEYPAD
static lv_indev_drv_t 		_lv_keys_drv;           /*Descriptor of a input device driver*/
uint32_t scan_keys();
void lv_keys_cb(lv_indev_drv_t * indev, lv_indev_data_t * data);
#endif

LGFX gfx;
static lv_disp_draw_buf_t 	_lv_draw_buf;
static lv_color_t 			_lv_color_buf[LV_BUF_SIZE];
static lv_disp_drv_t 		_lv_display_drv;        /*Descriptor of a display driver*/

SooghGUI::SooghGUI()
{

};

SooghGUI::~SooghGUI()
{

};

bool SooghGUI::begin()
{
    gfx.init();
    gfx.setRotation(1);
    gfx.setColorDepth(24);

	lv_init();

    lv_disp_draw_buf_init(&_lv_draw_buf, _lv_color_buf, NULL, LV_BUF_SIZE);

    lv_disp_drv_init(&_lv_display_drv);          /*Basic initialization*/
    _lv_display_drv.flush_cb = lv_disp_cb;    /*Set your driver function*/
    _lv_display_drv.draw_buf = &_lv_draw_buf;        /*Assign the buffer to the display*/
    _lv_display_drv.hor_res = DISPLAY_WIDTH;   /*Set the horizontal resolution of the display*/
    _lv_display_drv.ver_res = DISPLAY_HEIGHT;   /*Set the vertical resolution of the display*/
    lv_disp_drv_register(&_lv_display_drv);      /*Finally register the driver*/

#ifdef GUI_TOUCH
    // uint16_t calData[] = { 239, 3926, 233, 265, 3856, 3896, 3714, 308};
    // gfx.setTouchCalibrate(calData);
    lv_indev_drv_init(&_lv_touch_drv);             /*Basic initialization*/
    _lv_touch_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
    _lv_touch_drv.read_cb = lv_touchpad_cb;      /*Set your driver function*/
    lv_indev_drv_register(&_lv_touch_drv);         /*Finally register the driver*/
#endif // GUI_TOUCH

#ifdef GUI_KEYPAD
    lv_indev_drv_init(&_lv_keys_drv);             /*Basic initialization*/
    _lv_keys_drv.type = LV_INDEV_TYPE_KEYPAD;    /*Touch pad is a pointer-like device*/
    _lv_keys_drv.read_cb = lv_keys_cb;      /*Set your driver function*/
    lv_indev_drv_register(&_lv_touch_drv);         /*Finally register the driver*/
#endif // GUI_KEYPAD

	// Empty activity stack
	while(!_scrstack.empty())
		_scrstack.pop();

	// And put bootscreen on the bottom
	// ScreenPtr scr = std::make_shared<Screen>(*this);
	// pushScreen(scr);
  	// pushScreen(ScreenType::BOOT);

	return true;
};

void SooghGUI::loop()
{
    // TODO: Use LVGL to handle global keys/events
	// if(handle_global_keys(_keypad))
	// 	_event = KEY_NONE;

	// ActStack may not be empty
	if(_scrstack.size() == 0)
	{
		ERROR("ScreenStack empty, push(Screen)");
		ScreenPtr scr = std::make_shared<Screen>(*this);
		pushScreen(scr);
		// pushScreen(ScreenType::BOOT);
	};

	// Keeping this (smart) ptr here is important! It prevents pop() from deleting the 
	// Activity while in handle(). It will be deleted when scr goes out of scope too
	ScreenPtr scr = _scrstack.top();

	// Debug activity
#ifdef GUI_DEBUG
	static ScreenPtr prev_scr = nullptr;
	if(scr != prev_scr)
	{
		if(prev_scr == nullptr)
			DBG("GUI: <null> -> %s", scr->name());
		else
			DBG("GUI: %s -> %s", prev_scr->name(), scr->name());
		prev_scr = scr;
	};
#endif

    scr->loop();

	// LVGL ticker
    time_t now = millis();
	{
    	lv_tick_inc(now - _prv_tick);
    	_prv_tick = now;
	};
    lv_timer_handler();
};

// ScreenPtr SooghGUI::pushScreen(ScreenType scrtype, void* data)
// {
// 	ScreenPtr scr = NULL;
// 	switch(scrtype)
// 	{
// 		case ScreenType::BOOT:			scr = std::make_shared<BootScreen>(); break;
// 		case ScreenType::MAIN:			scr = std::make_shared<MainScreen>(); break;

// 		default:
// 			pushMessageScreen("Error:", __FUNCTION__, "Invalid <ScreenType>", " identifier"); 
// 			return NULL;
// 	};
//     pushScreen(scr, data);
// 	return scr;
// };

ScreenPtr SooghGUI::pushScreen(ScreenPtr scr, void* data)
{
	DBG("push(%s)", scr->name());
	_scrstack.push(scr);
    DBG("load(%s)", scr->name());
    scr->load();

	return scr;
};

void SooghGUI::popScreen(Screen* scr)
{
	// There must an activity on the stack to do this..
	if(_scrstack.empty())
		return;

	// ActivityPtr is a smart ptr. It will delete a in GUI::handle() eventually
	ScreenPtr top = _scrstack.top();
	_scrstack.pop();
	DBG("GUI: pop(%s)", top->name());

    // Just a check for now
    if(scr != nullptr && top.get() != scr)
    {
        ERROR("Screen* given does not match top().");
        // pushMessageScreen("ERROR", "Screen* != top()");
        return;
    };

	// make the screen below active again
    _scrstack.top()->load();

	// DBG("popped, will delete (eventually): %s(%p)", a->name(), a);
	return;
};

void lv_disp_cb(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p)
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    gfx.startWrite();
    gfx.setAddrWindow( area->x1, area->y1, w, h );
    gfx.writePixelsDMA((lgfx::rgb565_t *)&color_p->full, w * h);
    gfx.endWrite();

    lv_disp_flush_ready( disp );
};

#ifdef GUI_TOUCH
void lv_touchpad_cb(lv_indev_drv_t * indev, lv_indev_data_t * data)
{
    uint16_t touchX, touchY;
    if(gfx.getTouch( &touchX, &touchY))
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
    data->key = scan_keys();//last_key();            /*Get the last pressed or released key*/
    if(data->key != KEY_NONE) 
        data->state = LV_INDEV_STATE_PRESSED;
    else 
        data->state = LV_INDEV_STATE_RELEASED;
	return;
};
#endif // GUI_KEYPAD
