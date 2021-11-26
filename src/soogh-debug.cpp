#include "soogh-debug.h"

static void debug_event_input(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
    // lv_obj_t * obj = lv_event_get_target(e);
	switch(code)
	{
	    /** Input device events*/
		case LV_EVENT_PRESSED: DBG("PRESSED"); return;
		// LV_EVENT_PRESSING
		// LV_EVENT_PRESS_LOST
		case LV_EVENT_SHORT_CLICKED: DBG("SHORT_CLICKED"); return;
		case LV_EVENT_LONG_PRESSED: DBG("LONG_PRESSED"); return;
		case LV_EVENT_LONG_PRESSED_REPEAT: DBG("LONG_PRESSED_REPEAT"); return;
		case LV_EVENT_CLICKED: DBG("CLICKED"); return;
		case LV_EVENT_RELEASED: DBG("RELEASED"); return;
		// LV_EVENT_SCROLL_BEGIN
		// LV_EVENT_SCROLL_END
		// LV_EVENT_SCROLL
		// LV_EVENT_GESTURE
		case LV_EVENT_KEY: DBG("KEY"); return;
		case LV_EVENT_FOCUSED: DBG("FOCUSED"); return;
		// case LV_EVENT_DEFOCUSED: DBG("DEFOCUSED"); return;
		case LV_EVENT_LEAVE: DBG("LEAVE"); return;
		// case LV_EVENT_HIT_TEST: DBG("HIT_TEST"); return;

		// Special events
		case LV_EVENT_VALUE_CHANGED: DBG("VALUECHANGED"); return;
		case LV_EVENT_INSERT: DBG("INSERT"); return;
    	case LV_EVENT_REFRESH: DBG("REFRES"); return;
    	case LV_EVENT_READY: DBG("READY"); return;
    	case LV_EVENT_CANCEL: DBG("CANCEL"); return;

    	// Other events
    	case LV_EVENT_DELETE: DBG("DELETE"); return;

		default: return;
	};
};

static void debug_event_drawing(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
    // lv_obj_t * obj = lv_event_get_target(e);
	switch(code)
	{
    /** Drawing events*/
		case LV_EVENT_COVER_CHECK: DBG("COVER_CHECK"); return;
		case LV_EVENT_REFR_EXT_DRAW_SIZE: DBG("EXT_DRAW_SIZE"); return;
		case LV_EVENT_DRAW_MAIN_BEGIN: DBG("MAIN_BEGIN"); return;
		case LV_EVENT_DRAW_MAIN        : DBG("MAIN"); return;
		case LV_EVENT_DRAW_MAIN_END   : DBG("MAIN_END"); return;
		case LV_EVENT_DRAW_POST_BEGIN  : DBG("POST_BEGIN"); return;
		case LV_EVENT_DRAW_POST: DBG("POST"); return;
		case LV_EVENT_DRAW_POST_END: DBG("POST_END"); return;
		case LV_EVENT_DRAW_PART_BEGIN: DBG("PART_BEGIN"); return;
		case LV_EVENT_DRAW_PART_END: DBG("PART_END"); return;
		default: return;
	};
};

static void debug_event_special(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
    // lv_obj_t * obj = lv_event_get_target(e);
	switch(code)
	{
		// Special events
		case LV_EVENT_VALUE_CHANGED: DBG("VALUECHANGED"); return;
		case LV_EVENT_INSERT: DBG("INSERT"); return;
    	case LV_EVENT_REFRESH: DBG("REFRES"); return;
    	case LV_EVENT_READY: DBG("READY"); return;
    	case LV_EVENT_CANCEL: DBG("CANCEL"); return;

		default: return;
	};
};

static void debug_event_other(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
    // lv_obj_t * obj = lv_event_get_target(e);
	switch(code)
	{
    	// Other events
		case LV_EVENT_DELETE              : DBG("DELETE"); return;
		case LV_EVENT_CHILD_CHANGED       : DBG("CHILD_CHANGED"); return;
		case LV_EVENT_CHILD_CREATED       : DBG("CHILD_CREATED"); return;
		case LV_EVENT_CHILD_DELETED       : DBG("CHILD_DETELED"); return;
		case LV_EVENT_SCREEN_UNLOAD_START : DBG("SCREEN_UNLOAD_START"); return;
		case LV_EVENT_SCREEN_LOAD_START   : DBG("LOAD_START"); return;
		case LV_EVENT_SCREEN_LOADED       : DBG("LOADED"); return;
		case LV_EVENT_SCREEN_UNLOADED     : DBG("UNLOADED"); return;
		case LV_EVENT_SIZE_CHANGED        : DBG("SIZE_CHANGED"); return;
		case LV_EVENT_STYLE_CHANGED       : DBG("STYLE_CHANGED"); return;
		case LV_EVENT_LAYOUT_CHANGED      : DBG("LAYOUT_CHANGED"); return;
		case LV_EVENT_GET_SELF_SIZE       : DBG("GET_SELF_SIZE"); return;
		default: return;
	};
};
