
#include "soogh-screen.h"
#include "soogh-color.h"
#include "soogh-gui.h"

#ifdef SOOGH_DEBUG
    #include <tools-log.h>
    #define SOOGH_DBG     DBG
#else
    #define SOOGH_DBG(msg, ...)
#endif

Screen::Screen(SooghGUI &gui) : _gui(gui) 
{
	SOOGH_DBG("CONSTRUCT %s(%p)", this->name(), this);
    _screen = lv_obj_create(NULL);
    lv_obj_set_style_pad_all(_screen, 0, 0);
    lv_obj_set_style_border_width(_screen, 0, 0);
    lv_obj_set_style_radius(_screen, 0, 0);
};

Screen::~Screen() 
{ 
	SOOGH_DBG("DESTROY %s(%p)", this->name(), this); 
	lv_obj_del(_screen); _screen = nullptr;
};

void Screen::load()
{
    // FIXME: do this in gui.loop()
    lv_scr_load_anim(_screen, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false);
    // lv_scr_load(_screen);
};

void Screen::close() 
{
    // Pop me, but only if it IS me at the screen stack
    _gui.popScreen(this);
};

