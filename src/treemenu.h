#ifndef __TREEMENU_H
#define __TREEMENU_H

#include <soogh.h>

// #include <memory>
#include <list>

#include <tools-log.h>
#include <tools-nocopy.h>


/*
	MenuItem (drawable: draw_item, draw_open)
		SubMenu
			TreeMenu
		FloatItem
		SwitchItem
		ListItem
*/

class MenuItem;
class TreeMenu;

typedef void treemenu_cb_t(MenuItem* item, void* user_data);

typedef std::stack<lv_group_t*> GroupStack;
typedef std::list<MenuItem*> MenuItemArray;
class MenuItem //: public NonCopyable
{
	public:
		MenuItem(MenuItem *parent, const char *text = "<none>");
		virtual ~MenuItem();

		void close();
		void open();
		bool isOpen();
		void onClose(treemenu_cb_t *func, void* user_data = nullptr);
		// void onChange(treemenu_cb_t func, void* user_data = nullptr);

	protected:
		virtual void close_children();
		virtual void draw_btn(lv_obj_t *lv_list) = 0;
		virtual void draw_open() {};
		virtual void draw_close() {};
		friend class SubMenu;
		friend class TreeMenu;

		MenuItem* parent();
		TreeMenu* root();
		void appendChild(MenuItem* child);

	protected:
		MenuItem *_parent = nullptr;
		MenuItemArray _children;

		const char* _text;
		bool _open = false;
		// lv_obj_t *_obj = nullptr;

		// Callbacks
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

 		BooleanField(MenuItem *parent, const char *text, bool *b, booltype_t type = BOOLTYPE_SWITCH) 
		 	: MenuItem(parent, text), value(b), _type(type) {};

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
 		ActionField(MenuItem *parent, const char *text, treemenu_cb_t *func, void* data = nullptr);

	protected:
		void draw_btn(lv_obj_t *lv_list);

	private: // Callbacks
		static void click_cb(lv_event_t *e);
};

class FloatField : public MenuItem
{
 	public:
 		FloatField(MenuItem *parent, const char *text, float *f);

	public: // member vars
		float *value;
		float min_value = 0;
		float max_value = 10;
		int decimals = 2;

	protected:
		void draw_btn(lv_obj_t *lv_list);
		void draw_open();
		void draw_close();

	private: // Callbacks
		static void click_cb(lv_event_t *e);
		static void btns_cb(lv_event_t * e);
		int digits();
	private:
		//draw_btn
		// lv_obj_t *_btn = nullptr;

		//draw_open/close
		lv_obj_t *_btns = nullptr;
		lv_obj_t *_spinbox = nullptr;
		lv_obj_t *_btn_lbl = nullptr;
};

class SubMenu : public MenuItem
{
	public:
		SubMenu(SubMenu *parent, const char *text) : MenuItem(parent, text) {	};

		// Construct children
		MenuSeparator* 		addSeparator(const char* text);
		SubMenu* 			addSubMenu(const char* text);
		FloatField* 		addFloat(const char* name, float* f);
		ActionField*		addAction(const char* name, treemenu_cb_t func, void* data = nullptr);
		BooleanField*		addSwitch(const char* name, bool* );
		BooleanField*		addCheckbox(const char* name, bool* );

	protected:
		void draw_btn(lv_obj_t *lv_list);
		void draw_open();
		void draw_close();
		static void click_cb(lv_event_t *e);

		lv_obj_t *_list = nullptr;
		lv_obj_t *_btn = nullptr;
		lv_obj_t *_btn_img = nullptr;
};

class TreeMenu : public SubMenu
{
	public:
		TreeMenu() : SubMenu(nullptr, "<root>") { };
		~TreeMenu();


		enum menukey_t {
			KEY_NONE,
			KEY_LEFT = LV_KEY_LEFT,
			KEY_RIGHT = LV_KEY_RIGHT,
			KEY_ENTER = LV_KEY_ENTER,
			KEY_ESC = LV_KEY_ESC
		};

		void sendKey(menukey_t key);
		// void set_group(lv_group_t* group);
		// lv_group_t* get_group();

		lv_group_t* group_push();
		void group_pop();
		lv_group_t* group_top();
		void group_add(lv_obj_t* obj);

	protected:
		lv_group_t *_grp = nullptr;
		// lv_group_t *_cgrp = nullptr;
		GroupStack _grpstack;

		void draw_open();
		void draw_close();
		void draw_btn() {};
		static void close_cb(lv_event_t *e);

};

#endif // __TREEMENU_H