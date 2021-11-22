
#include "soogh-screen.h"
#include "soogh-color.h"
#include "soogh-gui.h"

Screen::Screen(SooghGUI &gui) : _gui(gui) 
{
	// DBG("CONSTRUCT %s(%p)", this->name(), this);
    _screen = lv_obj_create(NULL);
};

Screen::~Screen() 
{ 
	// DBG("DESTROY %s(%p)", this->name(), this); 
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
    _gui.popScreen(this);
};

