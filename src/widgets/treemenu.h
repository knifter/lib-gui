#ifndef __TREEMENU_H
#define __TREEMENU_H

#include <lvgl.h>
#include <list>
#include <stack>
#include <sys/types.h>

/*
	MenuItem (drawable: draw_item, draw_open)
		SubMenu
			TreeMenu
		..Fields
*/

class MenuItem;
class TreeMenu;

typedef void treemenu_cb_t(MenuItem* item, void* user_data);

typedef std::stack<lv_group_t*> GroupStack;
typedef std::list<MenuItem*> MenuItemArray;

class MenuItem
{
	public:
		MenuItem(MenuItem *parent, const char *text = nullptr);
		virtual ~MenuItem();

		void close();
		void open();
		bool isOpen();
		void onOpen(treemenu_cb_t *func, void* user_data = nullptr);
		void onClose(treemenu_cb_t *func, void* user_data = nullptr);
		void onChange(treemenu_cb_t func, void* user_data = nullptr);

	protected:
		virtual void close_children();
		virtual void draw_btn(lv_obj_t *lv_list) = 0;
		virtual void draw_open() {};
		virtual void draw_close() {};
		virtual bool handleKey(lv_key_t key, lv_obj_t* focused_obj);
		friend class SubMenu;
		friend class TreeMenu;

		MenuItem* parent();
		TreeMenu* root();
		void appendChild(MenuItem* child);
		virtual void call_onopen();
		virtual void call_onclose();
		virtual void call_onchange();

	protected:
		MenuItem *_parent = nullptr;
		MenuItemArray _children;

		const char* _text;
		bool _open = false;
		lv_obj_t* _obj = nullptr;

		// Callbacks
		treemenu_cb_t 	*_open_cb = nullptr;
		void 			*_open_data = nullptr;
		treemenu_cb_t 	*_close_cb = nullptr;
		void 			*_close_data = nullptr;
		treemenu_cb_t 	*_change_cb = nullptr;
		void 			*_change_data = nullptr;

	public:
		// TODO: disable copy constructor
};

class MenuSeparator : public MenuItem
{
	public:
		MenuSeparator(MenuItem *parent, const char *text) : MenuItem(parent, text) {};
		void set_text(const char* text);

	protected:
		void draw_btn(lv_obj_t *lv_list);
};

class BooleanField : public MenuItem
{
 	public:
		typedef enum
		{
			BOOLTYPE_SWITCH,
			BOOLTYPE_CHECKBOX
		} booltype_t;

 		BooleanField(MenuItem *parent, const char *name, bool *b, booltype_t type = BOOLTYPE_SWITCH) 
		 	: MenuItem(parent, name), value(b), _type(type) {};

		bool *value;

	protected:
		booltype_t _type;
		void draw_btn(lv_obj_t *lv_list);
		
	private: // Callbacks
		lv_obj_t *_sw;
		static void click_cb(lv_event_t *e);
};

class ActionField : public MenuItem
{
 	public:
 		ActionField(MenuItem *parent, const char *name, treemenu_cb_t *func, void* data = nullptr, const void* lv_icon = nullptr);

	protected:
		void draw_btn(lv_obj_t *lv_list);

	private: // Callbacks
		static void click_cb(lv_event_t *e);
		const void* _lv_icon;
};

class NumberField : public MenuItem
{
 	public:
		NumberField(MenuItem *parent, const char *name, double *f, double min = -10, double max = 10);

	public: // member vars
		double *value;
		double min_value = -10.0;
		double max_value = 10.0;
		int decimals = 2;
		
	protected:
		void draw_btn(lv_obj_t *lv_list);
		void draw_open();
		bool handleKey(lv_key_t key, lv_obj_t* focused_obj);
		void draw_close();
		void export_value();

	private: // Callbacks
		int digits();

		//draw_btn
		lv_obj_t *_btn = nullptr;
		static void btn_clicked_cb(lv_event_t *e);

		//draw_open/close
#ifdef SOOGH_TOUCH
		lv_obj_t *_btns = nullptr;
		static void btns_cb(lv_event_t * e);
#endif
		lv_obj_t *_spinbox = nullptr;
		static void spinbox_draw_cb(lv_event_t* e);
		lv_obj_t *_btn_lbl = nullptr;
		bool _edit;
		uint8_t _lastpos = 0xFF;
};

class SelectorField : public MenuItem
{
	public:
		// enum : uint8_t
		// {
		// 	FLAG_DISABLED = 0x01,
		// 	FLAG_HIDDEN = 0x02,			
		// };
		typedef struct 
		{
			int32_t id;
			const char* shortname;
			const char* longname;
			// uint8_t flags;
		} item_t;
		
		SelectorField(MenuItem *parent, const char *name, int32_t* target, const item_t *items);

	protected:
		// bool sendKey(lv_key_t key);
		void draw_btn(lv_obj_t *lv_list);
		void draw_open();
		bool handleKey(lv_key_t key, lv_obj_t* focused_obj);
		void draw_close();

	private:
		int32_t *_target;
		const item_t* _items;

		static void btn_click_cb(lv_event_t *e);
		static void choose_click_cb(lv_event_t *e);

		lv_obj_t *_btn = nullptr;
		lv_obj_t *_btn_lbl = nullptr;
		lv_obj_t *_list = nullptr;
};

// class DateField : public MenuItem
// {
//  	public:
// 		DateField(MenuItem *parent, const char *name, struct tm *target);

// 	public: // member vars
		
// 	protected:
// 		// bool sendKey(lv_key_t key);
// 		void draw_btn(lv_obj_t *lv_list);
// 		void draw_open();
// 		bool handleKey(lv_key_t key, lv_obj_t* focused_obj);
// 		void draw_close();

// 	private: // Callbacks
// 		void export_selected_value();

// 		struct tm* _target;

// 		// menu button
// 		lv_obj_t *_btn = nullptr;
// 		lv_obj_t *_btn_lbl = nullptr;
// 		static void btn_click_cb(lv_event_t *e);

// 		// When open
// 		lv_obj_t* _calendar = nullptr;
// 		lv_obj_t* _calendar_head = nullptr;
// 		static void calender_event_cb(lv_event_t * e);
// 		bool _edit;

// #ifdef SOOGH_TOUCH
// #endif
// };

// class TimeField : public MenuItem
// {
//  	public:
// 		TimeField(MenuItem *parent, const char *name, struct tm *target);

// 	public: // member vars
		
// 	protected:
// 		// bool sendKey(lv_key_t key);
// 		void draw_btn(lv_obj_t *lv_list);
// 		void draw_open();
// 		bool handleKey(lv_key_t key, lv_obj_t* focused_obj);
// 		void draw_close();

// 	private: // Callbacks
// 		void export_value();

// 		struct tm* _target;

// 		// menu button
// 		lv_obj_t *_btn = nullptr;
// 		lv_obj_t *_btn_lbl = nullptr;
// 		static void btn_click_cb(lv_event_t *e);

// 		// When open

// 		bool _edit;

// #ifdef SOOGH_TOUCH
// #endif
// };

class SubMenu : public MenuItem
{
	public:
		SubMenu(SubMenu *parent, const char *name) : MenuItem(parent, name) {	};

		// Construct children
		MenuSeparator* 		addSeparator(const char* optional_text = nullptr);
		SubMenu* 			addSubMenu(const char* name);
		NumberField* 		addSpinbox(const char* name, double* f, double min = -10, double max = 10, uint decimals = 2);
		ActionField*		addAction(const char* name, treemenu_cb_t func, void* data = nullptr, const void* lv_icon = nullptr);
		BooleanField*		addSwitch(const char* name, bool* );
		BooleanField*		addCheckbox(const char* name, bool* );
		SelectorField*		addSelector(const char* name, int32_t* seltarget, SelectorField::item_t *items);
		// DateField*			addDate(const char* name, struct tm* date_target);
		// TimeField*			addTime(const char* name, struct tm* time_target);

	protected:
		void draw_btn(lv_obj_t *lv_list);
		void draw_open();
		void draw_close();
		static void click_cb(lv_event_t *e);
		static void close_cb(lv_event_t *e);

		lv_obj_t *_list = nullptr;
		lv_obj_t *_btn = nullptr;
		lv_obj_t *_btn_img = nullptr;
};

class TreeMenu : public SubMenu
{
	public:
		TreeMenu() : SubMenu(nullptr, "<root>") { };
		~TreeMenu();

		bool sendKey(lv_key_t key);

		void addCloseMenuButton();

		// FIXME: make these protected
		static void close_menu_cb(MenuItem* item, void* user_data);
		lv_group_t* group_push();
		void group_pop();
		lv_group_t* group_top();
		void group_add(lv_obj_t* obj);

	protected:
		// lv_group_t *_grp = nullptr;
		GroupStack _grpstack;

		void draw_open();
		void draw_close();
		void draw_btn() {};

};

#endif // __TREEMENU_H