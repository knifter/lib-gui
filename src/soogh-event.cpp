#include "soogh-event.h"

#include <stdlib.h>
#include <string.h>

const char* soogh_event_name(soogh_event_t e)
{
	if(e == KEY_NONE)
		return "KEY_NONE";
	static char buf[32];
	strcpy(buf, "KEY_");
	if(e & KEY_A) strcat(buf, "A");
	if(e & KEY_B) strcat(buf, "B");
	if(e & KEY_C) strcat(buf, "C");
	if(e & KEYTOOL_SHORT) strcat(buf, "_SHORT");
	if(e & KEYTOOL_LONG) strcat(buf, "_LONG");
	if(e & KEYTOOL_LONG_REPEAT) strcat(buf, "_LONG_REPEAT");
	return buf;
};
