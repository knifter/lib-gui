#include "treemenu.h"

#include <soogh.h>

// #include <memory>
#include <vector>

#include <tools-log.h>
#include <tools-nocopy.h>


/*
	Node (Tree relations only)
		MenuItem (drawable: draw_item, draw_open)
			Menu
				Root(Menu)
			FloatItem
			SwitchItem
			ListItem

*/

/*** MenuItem ***************************************************************************************/
MenuItem::MenuItem(MenuItem *parent, const char *text) : _parent(parent), _text(text)
{
	// DBG("CONSTRUCT(%s) = %p", name, this);
	if(_parent)
		_parent->appendChild(this);
};

MenuItem::~MenuItem()
{
	// This will recursively(!) delete all children, bottom up.
	std::for_each(std::begin(_children), std::end(_children), [](MenuItem* child) 
	{
		delete child;
	});

	if(_obj != nullptr)
	{
		WARNING("Deleting stray _obj!");
		lv_obj_del(_obj);
	};
	// DBG("DESTROY(%s) = %p", _text, this);
};

MenuItem* MenuItem::parent()
{
	return _parent;
};

MenuItem* MenuItem::root()
{
	MenuItem* root = _parent;
	while(root->parent() != nullptr)
		root = root->parent();
	return root;
};

void MenuItem::appendChild(MenuItem* child)
{
	if(!child)
	{
		ERROR("child == null");
		return;
	};
	_children.push_back(child);
};

void MenuItem::close()
{
	if(_obj)
		lv_obj_del(_obj); 
	_obj = nullptr;
};

bool MenuItem::isOpen() 
{ 
	return _obj != nullptr; 
};

/*** Separator ***************************************************************************************/
void MenuSeparator::draw_btn(lv_obj_t *lv_list)
{
	lv_list_add_text(lv_list, _text);
};

/*** FloatItem ***************************************************************************************/
void FloatField::draw_btn(lv_obj_t *lv_list)
{
	// TODO: can this be moved to MenuItem?
	_btn = lv_list_add_btn(lv_list, nullptr, _text);
	lv_obj_add_event_cb(_btn, click_cb, LV_EVENT_CLICKED, this);
	lv_obj_set_flex_flow(_btn, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_style_pad_row(_btn, 3, 0);

	_btn_lbl = lv_label_create(_btn);
	lv_label_set_text_fmt(_btn_lbl, "%.02f", *value);
	lv_obj_set_flex_grow(_btn_lbl, 1);
	lv_obj_set_style_text_color(_btn_lbl, COLOR_GREY, 0);
};

void FloatField::open()
{
	if(_obj)
		return;
	
	// draw spinbox exactly over the label
	_spinbox = lv_spinbox_create(_btn_lbl);
	{
		lv_coord_t h = lv_obj_get_height(_btn_lbl);
		lv_coord_t w = lv_obj_get_width(_btn_lbl);
		lv_obj_set_size(_spinbox, w, h);

		// lv_obj_set_flex_grow(_spinbox, 4);
		// lv_obj_center(_spinbox);

		lv_spinbox_set_range(_spinbox, -1000, 25000);
		lv_spinbox_set_digit_format(_spinbox, 4, 2);
		lv_spinbox_set_value(_spinbox, *value * 100);
		lv_spinbox_step_prev(_spinbox);
		lv_spinbox_step_prev(_spinbox);
	};

	// And floating buttons just below the spinbox
	_obj = lv_btnmatrix_create(lv_layer_top());
	lv_coord_t y = lv_obj_get_y2(_spinbox);
	lv_coord_t x = lv_obj_get_x(_spinbox);
	lv_coord_t w = DISPLAY_WIDTH - x;
	lv_coord_t h = LV_PCT(30);
	
	lv_obj_set_size(_obj, w, h);
	lv_obj_set_pos(_obj, x, y);
	lv_obj_set_style_pad_all(_obj, 3, 0);

	static const char * map[] = {
		LV_SYMBOL_LEFT, 
		LV_SYMBOL_MINUS, 
		LV_SYMBOL_OK, 
		LV_SYMBOL_PLUS,
		LV_SYMBOL_RIGHT, "" };
	lv_btnmatrix_set_map(_obj, map);
	lv_btnmatrix_set_btn_width(_obj, 2, 2);
	lv_obj_align(_obj, LV_ALIGN_CENTER, 0, 20);
	lv_obj_add_event_cb(_obj, btns_cb, LV_EVENT_VALUE_CHANGED, this);
};

void FloatField::close()
{
	lv_label_set_text_fmt(_btn_lbl, "%.02f", *value);
	lv_obj_del(_spinbox);
	MenuItem::close();
};

/* static */ void FloatField::click_cb(lv_event_t *e) // static
{
	FloatField* me = static_cast<FloatField*>(e->user_data);
	me->open();
};
/* static */ void FloatField::btns_cb(lv_event_t * e)
{
	// lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * obj = lv_event_get_target(e);
	uint32_t id = lv_btnmatrix_get_selected_btn(obj);
	FloatField* me = static_cast<FloatField*>(e->user_data);
	switch(id)
	{
		case 0: lv_spinbox_step_prev(me->_spinbox); break;
		case 1: lv_spinbox_decrement(me->_spinbox); break;
		case 2: DBG("Close"); me->close(); break;
		case 3: lv_spinbox_increment(me->_spinbox); break;
		case 4: lv_spinbox_step_next(me->_spinbox); break;
		default: DBG("ID = %d", id); break;
	};
	*(me->value) = lv_spinbox_get_value(me->_spinbox) / 100.0;
};

/*** SubMenu ***************************************************************************************/
void SubMenu::open()
{
	// If obj then already open
	if(_obj)
		return;

	lv_obj_t *lv_parent = lv_layer_top();

	// list
	if(_obj)
	{
		WARNING("Free prev _obj!");
		lv_obj_del(_obj);
	};

	_obj = lv_list_create(lv_parent);
	lv_obj_align(_obj, LV_ALIGN_LEFT_MID, 0, 0);
	lv_obj_set_size(_obj, LV_PCT(50), LV_PCT(100));

	draw_first_btn(_obj); // "back" for, "close" for Root

	// TODO: dismantle lambda into static member so draw_item can be protected
	std::for_each(std::begin(_children), std::end(_children), 
		[this](MenuItem* child) 
		{
			child->draw_btn(this->_obj);
		});
};

MenuSeparator* SubMenu::addSeparator(const char* text)
{
	return new MenuSeparator(this, text);
};

SubMenu* SubMenu::addSubMenu(const char* text)
{
	SubMenu *child = new SubMenu(this, text);
	// appendChild(child);
	return child;
};

FloatField* SubMenu::addFloat(const char* name, float* f)
{
	FloatField* item = new FloatField(this, name, f);
	return item;
};

void SubMenu::draw_first_btn(lv_obj_t *lv_list)
{
	// Exit button
	lv_obj_t *btn = lv_list_add_btn(lv_list, LV_SYMBOL_LEFT, "Back");
	lv_obj_add_event_cb(btn, SubMenu::close_cb, LV_EVENT_CLICKED, this);
};

void SubMenu::draw_btn(lv_obj_t *lv_list)
{
	lv_obj_t *btn = lv_list_add_btn(lv_list, LV_SYMBOL_RIGHT, _text);
	btn = btn;
};

/*static*/ void SubMenu::close_cb(lv_event_t *e)
{
	// Root* me = reinterpret_cast<Root*>(e->user_data);
	SubMenu* me = static_cast<SubMenu*>(e->user_data);
	me->close();
};

/*** Root ***************************************************************************************/
void TreeMenu::draw_first_btn(lv_obj_t *lv_list)
{
	// Exit button
	lv_obj_t *btn = lv_list_add_btn(lv_list, LV_SYMBOL_CLOSE, "Close");
	lv_obj_add_event_cb(btn, SubMenu::close_cb, LV_EVENT_CLICKED, this);
};

