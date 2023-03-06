#ifndef __SOOGH_SCREEN_H
#define __SOOGH_SCREEN_H

#include <stack>
#include <memory>

class Screen;

typedef std::shared_ptr<Screen> ScreenPtr;
typedef std::stack<ScreenPtr> ScreenStack;

#include "soogh-lgfx.h"
#include "soogh-gui.h"
#include "soogh-event.h"

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

		virtual void init() {};
        virtual void load();
		virtual bool handle(soogh_event_t e) { return false; };
        virtual void loop() { return; };
		virtual void close();

		virtual const char* name() { return ScreenNames[(int) type()]; };
	protected:
		SooghGUI& 	_gui;
        lv_obj_t*   _screen;
	
	public:
		Screen(const Screen&) = delete;
	    Screen& operator=(Screen const&) = delete;
};


#endif //__SOOGH_SCREEN_H
