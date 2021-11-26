#include "soogh.h"

#define DEBUG_DEVICE        Serial
#define DBG(msg, ...)      DEBUG_DEVICE.printf("%s.%s: " msg "\n", __FILE__, __FUNCTION__, ##__VA_ARGS__)

void debug_event_input(lv_event_t * e);
void debug_event_drawing(lv_event_t * e);
void debug_event_special(lv_event_t * e);
void debug_event_other(lv_event_t * e);
