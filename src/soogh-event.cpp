#include "soogh-event.h"

#include <stdlib.h>
#include <string.h>

const char* soogh_event_name(soogh_event_t e)
{
	if(e == KEY_NONE)
		return "KEY_NONE";
	if(e == KEY_RELEASED)
		return "KEY_RELEASED";

    static char buf[sizeof("KEY_ABC_SHORT_LONG_REPEAT_RELEASED")+1];
	strcpy(buf, "KEY_");                                        // 4
	if(e & KEY_A)               strcat(buf, "A");               // 1
	if(e & KEY_B)               strcat(buf, "B");               // 1
	if(e & KEY_C)               strcat(buf, "C");               // 1
    // if(e & KEYTOOL_PRESSED)     strcat(buf, "_PRESSED");        // 7
	if(e & KEYTOOL_SHORT)       strcat(buf, "_SHORT");          // 6
	if(e & KEYTOOL_LONG)        strcat(buf, "_LONG");           // 5
	if(e & KEYTOOL_LONG_REPEAT) strcat(buf, "_LONG_REPEAT");    // 12
    if(e & KEYTOOL_RELEASED)    strcat(buf, "_RELEASED");       // 9
	return buf;
};
