#ifndef __SOOGH_GUI_H
#define __SOOGH_GUI_H

#include <stack>
#include <memory>

#include "soogh-conf.h"
#include "soogh-lgfx.h"
#include "soogh-screen.h"

class Screen;
enum class ScreenType;

typedef std::shared_ptr<Screen> ScreenPtr;
typedef std::stack<ScreenPtr> ScreenStack;

class SooghGUI
{
	public:
		SooghGUI();
		~SooghGUI();

		virtual bool begin();
		virtual time_t loop();

#ifdef GUI_KEYPAD
		virtual uint32_t scan_keys() = 0;
#endif

		// virtual ScreenPtr	pushScreen(ScreenType, void* data = nullptr);
		virtual ScreenPtr	pushScreen(ScreenPtr, void* data = nullptr);
		virtual void		popScreen(Screen* = nullptr);

		virtual void showMessage(const char* title, const char* text);

	private:
		ScreenStack			_scrstack;
		time_t				_prv_tick;
		lv_obj_t		*_msgbox = nullptr;

		// LVGL

	public:
		SooghGUI(const SooghGUI&) = delete;
	    SooghGUI& operator=(SooghGUI const&) = delete;
};

#endif // __SOOGH_GUI_H