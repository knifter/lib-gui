#ifndef __SOOGH_GUI_H
#define __SOOGH_GUI_H

#include <stack>
#include <memory>

#include "soogh-conf.h"
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

		bool begin();
		void loop();

#ifdef GUI_KEYPAD
		virtual uint32_t scan_keys() = 0;
#endif

		// ScreenPtr	pushScreen(ScreenType, void* data = nullptr);
		ScreenPtr	pushScreen(ScreenPtr, void* data = nullptr);
		void		popScreen(Screen* = nullptr);

	private:
		ScreenStack			_scrstack;
		time_t				_prv_tick;
		
		// LVGL

	public:
		SooghGUI(const SooghGUI&) = delete;
	    SooghGUI& operator=(SooghGUI const&) = delete;
};

#endif // __SOOGH_GUI_H