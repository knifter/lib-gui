#ifndef __SOOGH_SCREEN_H
#define __SOOGH_SCREEN_H

#include <LGFX_TFT_eSPI.hpp>
#include <lvgl.h>

#include "soogh-gui.h"

enum class ScreenType
{
	BASE,
	BOOT,
	MAIN,
	MENU,
	MESSAGE,

	// POWEROFF,
	// BATTLOW,
	// MESSAGE,
	// OTA,
	_NUM
};

constexpr const char* ScreenNames[static_cast<int>(ScreenType::_NUM)] = 
{
	"BASE",
	"BOOT",
	"MAIN",
	"MENU",
	"MESSAGE"

	// "SCREENSAVE",
	// "POWEROFF",
	// "MESSAGE",
	// "OTA"
};

class SooghGUI;

class Screen
{
    public:
        Screen(SooghGUI &gui);

		virtual ~Screen();
		virtual ScreenType type() { return ScreenType::BASE; };

        void load();
		virtual void close();
		virtual void init() {};
        virtual bool loop() { return true; };

		virtual const char* name() { return ScreenNames[(int) type()]; };
	protected:
		SooghGUI& 	_gui;
        lv_obj_t*   _screen;
	
	public:
		Screen(const Screen&) = delete;
	    Screen& operator=(Screen const&) = delete;
};


#endif //__SOOGH_SCREEN_H