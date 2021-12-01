#ifndef __SOOGH_GUI_H
#define __SOOGH_GUI_H

#include <stack>
#include <memory>

typedef uint32_t soogh_event_t;
class SooghGUI;

#include "soogh-conf.h"
#include "soogh-lgfx.h"
#include "soogh-screen.h"

class Screen;
enum class ScreenType;

class SooghGUI
{
	public:
		SooghGUI();
		~SooghGUI();

		virtual bool begin();
		virtual time_t loop();

		virtual bool handle(soogh_event_t e) { return false; };

		virtual ScreenPtr	pushScreen(ScreenPtr, void* data = nullptr);
		virtual void		popScreen(Screen* = nullptr);

		virtual void showMessage(const char* title, const char* text);

	protected:
		ScreenStack			_scrstack;
		time_t				_prv_tick;
		lv_obj_t			*_msgbox = nullptr;

	public:
		SooghGUI(const SooghGUI&) = delete;
	    SooghGUI& operator=(SooghGUI const&) = delete;
};

#endif // __SOOGH_GUI_H