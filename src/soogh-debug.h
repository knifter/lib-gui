#include "soogh.h"

#ifndef DBG
	#define DEBUG_DEVICE        Serial
	#define DBG(msg, ...)      	DEBUG_DEVICE.printf("%s.%s: " msg "\n", __FILE__, __FUNCTION__, ##__VA_ARGS__)
#endif

void debug_event_input(lv_event_t * e);
void debug_event_input_info(lv_event_t * e);
void debug_event_drawing(lv_event_t * e);
void debug_event_special(lv_event_t * e);
void debug_event_other(lv_event_t * e);

#define DEBUG_EVENT_INPUT(obj)  	lv_obj_add_event_cb(obj, debug_event_input_info, LV_EVENT_ALL, (void*)(#obj));
