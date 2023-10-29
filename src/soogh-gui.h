#ifndef __SOOGH_GUI_H
#define __SOOGH_GUI_H

#include <stack>
#include <memory>


#include "soogh-conf.h"
#include "soogh-lgfx.h"
#include "soogh-screen.h"
#include "soogh-event.h"

typedef std::stack<lv_group_t*> GroupStack;

class Screen;

class SooghGUI
{
	public:
		SooghGUI();
		~SooghGUI();

		virtual bool begin();
		virtual bool handle(soogh_event_t e);
		void flushEvents();

		virtual ScreenPtr	pushScreen(ScreenPtr, void* data = nullptr);
		virtual void		popScreen(Screen* = nullptr);

		void 	pushGroup(lv_group_t*);
		void	popGroup();

		virtual void showMessage(const char* title, const char* text, lv_event_cb_t onclose = nullptr);

	protected:
		ScreenStack			_scrstack;
		GroupStack  		_groupstack;
		time_t				_prv_tick;
		lv_obj_t			*_msgbox = nullptr;
        bool                _ignore_events = false;

	public:
		virtual time_t loop();

		SooghGUI(const SooghGUI&) = delete;
	    SooghGUI& operator=(SooghGUI const&) = delete;
};

#endif // __SOOGH_GUI_H