#include "soogh-gui.h"

#include <lvgl.h>

#include "soogh-conf.h"
#include "soogh-screen.h"
#include "soogh-event.h"
#include "soogh-lgfx.h"

#include <tools-log.h>
#ifdef SOOGH_DEBUG
    #define SOOGH_DBG     DBG
#else
    #define SOOGH_DBG(msg, ...)
#endif

SooghGUI::SooghGUI()
{
};

SooghGUI::~SooghGUI()
{

};

bool SooghGUI::begin()
{
	lgfx_init();
	lvgl_init();
	
	// Empty screen stack
	while(!_scrstack.empty())
		_scrstack.pop();

	// And put bootscreen on the bottom
	// ScreenPtr scr = std::make_shared<Screen>(*this);
	// pushScreen(scr);
  	// pushScreen(ScreenType::BOOT);

	return true;
};

time_t SooghGUI::loop()
{
	// ScreenStack may not be empty
	if(_scrstack.size() == 0)
	{
		ERROR("ScreenStack empty, push(Screen)");
		// pushScreen(ScreenType::BOOT);
		ScreenPtr scr = std::make_shared<Screen>(*this);
		pushScreen(scr);
	};

	// Keeping this (smart) ptr here is important! It prevents pop() from deleting the 
	// Activity while in handle(). It will be deleted when scr goes out of scope too
	ScreenPtr scr = _scrstack.top();

	// Debug activity
#ifdef SOOGH_DEBUG
	static ScreenPtr prev_scr = nullptr;
	if(scr != prev_scr)
	{
		if(prev_scr == nullptr)
			DBG("GUI: <none> -> %s", scr->name());
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
    return lv_timer_handler();
};

void SooghGUI::flushEvents()
{
	// Start ignoring events until KEY_RELEASED is transmitted. 
    SOOGH_DBG("Disabling events.");
	_ignore_events = true;
};

bool SooghGUI::handle(soogh_event_t e)
{
	loop();

    // flush/ignore all events if a screen has closed previously, wait for KEY_RELEASED
    if(_ignore_events)
    {
        if(e != KEY_RELEASED)
        {
            SOOGH_DBG("Discard/Flush event: %s", soogh_event_name(e));
            return true;
        };
        SOOGH_DBG("Re-enabling events.");
        _ignore_events = false;
        return true;
    };
    
    // Handle global events
    switch(e)
    {
        case KEY_A_PRESSED:
        case KEY_B_PRESSED:
        case KEY_C_PRESSED:
            if(_msgbox)
            {
				// make close cb be called & close already
				lv_event_send(_msgbox, LV_EVENT_VALUE_CHANGED, lvgl_indev_keyenc);
                lv_msgbox_close(_msgbox); 
				_msgbox = nullptr;
                SOOGH_DBG("_msgbox closed.");
                return true;
            };
        default: break;
    };

    // See if the Screen handles it
   	ScreenPtr scr = _scrstack.top();
    if(scr->handle(e))
    {
        SOOGH_DBG("Event %s handled by screen(%s)", soogh_event_name(e), scr->name());
        return true;
    };
    SOOGH_DBG("Event %s NOT handled by screen(%s)", soogh_event_name(e), scr->name());

    // Give the bare keys to LVGL
    // switch(e)
    // {
    //     case KEY_A:     DBG("A"); lvgl_enc_pressed = true;  lvgl_enc_last_key = LV_KEY_LEFT ; break;
    //     case KEY_B:     DBG("B"); lvgl_enc_pressed = true;  lvgl_enc_last_key = LV_KEY_ENTER; break;
    //     case KEY_C:     DBG("C"); lvgl_enc_pressed = true;  lvgl_enc_last_key = LV_KEY_RIGHT; break;
    //     case KEY_AC:    lvgl_enc_pressed = true;  lvgl_enc_last_key = LV_KEY_BACKSPACE; break;
    //     default:  		lvgl_enc_pressed = false;
    // };

    return true;
};

ScreenPtr SooghGUI::pushScreen(ScreenPtr scr, void* data)
{
	SOOGH_DBG("GUI: Push %s(%p=%p)", scr->name(), scr, scr.get());
	_scrstack.push(scr);
	// GUI_DBG("GUI: Load %s(%p)", scr->name(), scr);
    scr->load();

	return scr;
};

void SooghGUI::popScreen(Screen* scr)
{
	// There must an activity on the stack to do this..
	if(_scrstack.empty())
		return;

    // Get current (Screen on) top
	ScreenPtr top = _scrstack.top();

    // Just a check for now
    if(scr != nullptr && top.get() != scr)
    {
        SOOGH_DBG("Screen* given does not match top().");
        return;
    };

	// ScreenPtr is a smart ptr. It will delete a in GUI::handle() eventually
	_scrstack.pop();
	SOOGH_DBG("pop(%s)", top->name());

    // Empty stack protection
	if(_scrstack.empty())
	{
		ERROR("Empty ScreenStack. push(BOOT).");
        showMessage("ERROR", "ScreenStack empty! push(BOOT)");
		// pushScreen(ScreenType::BOOT);
		ScreenPtr scr = std::make_shared<Screen>(*this);
		pushScreen(scr);
	};

	// make the screen below active again
    _scrstack.top()->load();

    // Make the gui.handle() flush events before next screen gets them
	flushEvents();

	SOOGH_DBG("popped, will delete (eventually): %s(%p=%p)", top->name(), top, top.get());
	return;
};

void SooghGUI::pushGroup(lv_group_t *grp)
{
	_groupstack.push(grp);
	lv_indev_set_group(lvgl_indev_keyenc, grp);
};

void SooghGUI::popGroup()
{
	if(_scrstack.empty())
	{
		WARNING("POP(group) on empty stack.");
		lv_indev_set_group(lvgl_indev_keyenc, nullptr);
		return;
	};

	_groupstack.pop();
	lv_indev_set_group(lvgl_indev_keyenc, _groupstack.top());
};

void SooghGUI::showMessage(const char* title, const char* text, lv_event_cb_t onclose)
{
    static const char * btns[] ={""};

    // Close the previous, if still one open
    if(_msgbox)
    {
        SOOGH_DBG("Destroying previous message box.");
        lv_msgbox_close(_msgbox);
    };

    //lv_layer_top()
    _msgbox = lv_msgbox_create(NULL, title, text, btns, true);
	if(onclose)
	{
		DBG("Adding onclose!");
    	lv_obj_add_event_cb(_msgbox, onclose, LV_EVENT_VALUE_CHANGED, NULL);
	};
    // lv_obj_t *but = lv_msgbox_get_btns(mbox1);
    lv_obj_center(_msgbox);
};

