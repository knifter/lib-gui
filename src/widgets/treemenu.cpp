#include "treemenu.h"

#include <soogh.h>

#include <tools-log.h>
#include <soogh-debug.h>
#include <math.h>

/*
Class hierarchy:
	MenuItem 			(drawable: draw_item, draw_open)
		SubMenu 		(actual menu)
			TreeMenu	(Top most Menu)
		NumberField 	(number editor)
		SwitchField 	(on/off boolean)
		ListField 		(picklist)
*/

/*** MenuItem ***************************************************************************************/
MenuItem::MenuItem(MenuItem *parent, const char *text) : _parent(parent), _text(text)
{
	// DBG("CONSTRUCT(%s) = %p", _text, this);
	if(_parent)
		_parent->appendChild(this);
};

MenuItem::~MenuItem()
{
	// This will recursively(!) delete all children, bottom up.
	for(auto child: _children)
		delete child;

	if(_open)
	{
		WARNING("Destroying open object.");
	};
	// DBG("DESTROY(%s) = %p", _text, this);
};

MenuItem* MenuItem::parent()
{
	return _parent;
};

TreeMenu* MenuItem::root()
{
	MenuItem* root = this;
	while(root->_parent != nullptr)
		root = root->_parent;
	return static_cast<TreeMenu*>(root);
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

void MenuItem::open()
{
	if(_open)
		return;
	// DBG("open(%s)", _text);
	// close other opened siblings
	if(_parent)
		_parent->close_children();

	draw_open();
	_open = true;
};

void MenuItem::close_children()
{
	// DBG("close_children(of %s)", _text);
	// propagate close through all children as well
	for(auto child: _children)
	{
		if(child->isOpen())
		{
			// DBG("%s: close child: %s", this->_text, child->_text);
			child->close();
		};
	};

};

bool MenuItem::handleKey(lv_key_t key, lv_obj_t*)
{	
	if(_parent)
		return root()->sendKey(key);
	return false;
};

void MenuItem::close()
{
	if(!_open)
		return;

	// DBG("close(%s)", _text);

	// make sure children are closed
	close_children();

	draw_close();
	
	// Call on-close event callback
	call_onclose();

	// and then me
	_open = false;
};

bool MenuItem::isOpen() 
{ 
	return _open; 
};

void MenuItem::onClose(treemenu_cb_t *func, void* user_data) 
{ 
	_close_cb = func;
	_close_data = user_data; 
};
inline void MenuItem::call_onclose()
{
	if(_close_cb)
		_close_cb(this, _close_data);
};

void MenuItem::onChange(treemenu_cb_t *func, void* user_data) 
{ 
	_change_cb = func;
	_change_data = user_data; 
};
inline void MenuItem::call_onchange()
{
	if(_change_cb)
		_change_cb(this, _change_data);
};

/*** Separator ***************************************************************************************/
void MenuSeparator::draw_btn(lv_obj_t *lv_list)
{
	if(_text == nullptr)
	{
		_obj = lv_list_add_text(lv_list, "");
		lv_obj_set_height(_obj, 3);
	}else{
    	_obj = lv_list_add_text(lv_list, _text);
		// lv_obj_set_height(_obj, 10);
	};
};

void MenuSeparator::set_text(const char* text) {
    if(_obj != nullptr) {
        lv_label_set_text(_obj, text);
    }
};

/*** SubMenu ***************************************************************************************/
void SubMenu::draw_open()
{
	lv_obj_t *lv_parent = _btn;

    lv_img_set_src(_btn_img, LV_SYMBOL_DOWN);

	lv_group_t* grp = root()->group_push();
	lv_group_set_editing(grp, false);

	// The mnu is a list
	_list = lv_list_create(lv_parent);
	lv_obj_set_style_border_width(_list, 0, 0);
	lv_obj_set_height(_list, 45*_children.size());

	// Draw first (back) button
	lv_obj_t *btn = lv_list_add_btn(_list, LV_SYMBOL_LEFT, "Back");
	lv_obj_add_event_cb(btn, SubMenu::close_cb, LV_EVENT_CLICKED, this);
	root()->group_add(btn);

	for(auto child: _children)
		child->draw_btn(_list);
};

void SubMenu::draw_close()
{
	root()->group_pop();
	lv_obj_del(_list); _list = nullptr;

	lv_img_set_src(_btn_img, LV_SYMBOL_RIGHT);
};

void SubMenu::draw_btn(lv_obj_t *lv_list)
{
	// construct btn manually to be able to change icon
	_btn = lv_list_add_btn(lv_list, nullptr, nullptr);
    _btn_img = lv_img_create(_btn);
    lv_img_set_src(_btn_img, LV_SYMBOL_RIGHT);
	lv_obj_t * label = lv_label_create(_btn);
	lv_label_set_text(label, _text);
	lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_obj_set_flex_grow(label, 1);

	lv_obj_set_flex_flow(_btn, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_add_event_cb(_btn, click_cb, LV_EVENT_CLICKED, this);

	root()->group_add(_btn);
};
/* static */ void SubMenu::click_cb(lv_event_t *e) // static
{
	auto me = static_cast<SubMenu*>(e->user_data);
	if(me->_open)
		me->close();
	else
		me->open();
};

/*static*/ void SubMenu::close_cb(lv_event_t *e)
{
	SubMenu* me = static_cast<SubMenu*>(e->user_data);
	me->close();
};

void TreeMenu::close_menu_cb(MenuItem* item, void* user_data) {
    SubMenu* me = static_cast<SubMenu*>(user_data);
    me->close();
}

void TreeMenu::addCloseMenuButton()
{
    addAction("Close", TreeMenu::close_menu_cb, this, LV_SYMBOL_CLOSE);
}

MenuSeparator* SubMenu::addSeparator(const char* name)
{
	return new MenuSeparator(this, name);
};

SubMenu* SubMenu::addSubMenu(const char* name)
{
	return new SubMenu(this, name);
};

NumberField* SubMenu::addSpinbox(const char* name, double* f, double min, double max, uint decimals)
{
	auto item = new NumberField(this, name, f, min, max);
	item->decimals = decimals;
	return item;
};

ActionField* SubMenu::addAction(const char* name, treemenu_cb_t *func, void *data, const void* lv_icon)
{
    return new ActionField(this, name, func, data, lv_icon);
};

BooleanField* SubMenu::addSwitch(const char* name, bool *b)
{
	return new BooleanField(this, name, b, BooleanField::BOOLTYPE_SWITCH);
};

BooleanField* SubMenu::addCheckbox(const char* name, bool *b)
{
	return new BooleanField(this, name, b, BooleanField::BOOLTYPE_CHECKBOX);
};

SelectorField*	SubMenu::addSelector(const char* name, int32_t* seltarget, SelectorField::item_t *items)
{
	return new SelectorField(this, name, seltarget, items);
};

/*** Root ***************************************************************************************/
TreeMenu::~TreeMenu()
{
	// We need to close (remove widgets) the menu before free-ing it
	// But needs to be done here on the root menu and derived class: the vtable is gone in ~MenuItem
	close();

	// free/del stacked groups, shouldn't happen if properly balanced
	while(!_grpstack.empty())
	{
		WARNING("free-ing stray lv_group_t from stack!");
		lv_group_del(_grpstack.top());
		_grpstack.pop();
	};
};

void TreeMenu::draw_open()
{

	lv_obj_t *lv_parent = lv_layer_top();

	lv_group_t* grp = group_push();
	lv_group_set_editing(grp, false);

	// The mnu is a list
	_list = lv_list_create(lv_parent);
	lv_obj_align(_list, LV_ALIGN_LEFT_MID, 0, 0);
	lv_obj_set_size(_list, LV_PCT(80), LV_PCT(100));

	for(auto child: _children)
		child->draw_btn(_list);
};

void TreeMenu::draw_close()
{
	lv_obj_del(_list); _list = nullptr;

	group_pop();
};


lv_group_t* TreeMenu::group_push()
{
	lv_group_t* g = lv_group_create();
	_grpstack.push(g);
	lv_indev_set_group(lvgl_indev_keyenc,  _grpstack.top());
	return g;
};

void TreeMenu::group_pop()
{
	if(_grpstack.empty())
	{
		WARNING("Can't pop group from shallow stack.");
		return;
	};
	
	lv_group_del(_grpstack.top());
	_grpstack.pop();

	if(!_grpstack.empty())
		lv_indev_set_group(lvgl_indev_keyenc,  _grpstack.top());
};

lv_group_t* TreeMenu::group_top()
{
	if(_grpstack.empty())
	{
		ERROR("No group on top() to give. Returning NULL!");
		return nullptr;
	};
	
	return _grpstack.top();
};

void TreeMenu::group_add(lv_obj_t* obj)
{
	if(_grpstack.empty())
	{
		WARNING("No group to add to.");
		return;
	};

	lv_group_add_obj(_grpstack.top(), obj);
};

bool TreeMenu::sendKey(lv_key_t key)
{
    if(!isOpen())
    {
        WARNING("Menu is closed.");
        return false;
    };
	if(_grpstack.empty())
	{
		WARNING("No group to send key to.");
		return false;
	};
	lv_group_t* grp = _grpstack.top();

	bool editable_or_scrollable = true;
	lv_obj_t *obj = lv_group_get_focused(grp);
	if(!obj)
	{
		WARNING("No obj focussed");
		return false;
	};

	// See if the item owning the object wants to handle the event
	if(obj->user_data)
	{
		MenuItem* item = static_cast<MenuItem*>(obj->user_data);
		if(item->handleKey(key, obj))
		{
			return true;
		};
	};

	// simulated indev mode
	bool editable_or_scrollable = lv_obj_is_editable(obj) || lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
	// DBG("edit_or_scrollable = %s, group.editing = %s", editable_or_scrollable ? "true": "false", lv_group_get_editing(grp) ? "true":"false");
	switch(key)
	{
		case LV_KEY_LEFT:
			if(lv_group_get_editing(grp))
			{
				// DBG("group.send(LEFT)");
				lv_group_send_data(grp, LV_KEY_LEFT);
			}else{
				// DBG("group.prev");
				lv_group_focus_prev(grp);
			};
			break;

		case LV_KEY_RIGHT:
			if(lv_group_get_editing(grp))
			{
				// DBG("group.send(RIGHT)");
				lv_group_send_data(grp, LV_KEY_RIGHT);
			}else{
				// DBG("group.next");
				lv_group_focus_next(grp);
			};
			break;

		case LV_KEY_ENTER:
			// PRESSED, RELEASE code from lv_indec.c(596).indev_encoder_proc()
			if(!editable_or_scrollable)
			{
				// DBG("!edit|scrollable: obj.send(PRESSED, RELEASED, SHORT_CLICKED, CLICKED)");
				lv_event_send(obj, LV_EVENT_PRESSED, lvgl_indev_keyenc);
				lv_event_send(obj, LV_EVENT_RELEASED, lvgl_indev_keyenc);
				lv_event_send(obj, LV_EVENT_SHORT_CLICKED, lvgl_indev_keyenc);
				lv_event_send(obj, LV_EVENT_CLICKED, lvgl_indev_keyenc);

				// lv_group_send_data(grp, LV_KEY_ENTER); // FIXME: Wasnt here orig
				break;
			};
			if(lv_group_get_editing(grp))
			{
				// DBG("obj.send(PRESSED)");
				lv_event_send(obj, LV_EVENT_PRESSED, lvgl_indev_keyenc);
				//if !long_press_sent || lv_group_object_count(g) <= 1
				if(lv_group_get_obj_count(grp) < 2)
				{
					// DBG("obj.send(RELEASED, SHORT_CLICKED, CLICKED)");
					lv_event_send(obj, LV_EVENT_RELEASED, lvgl_indev_keyenc);
					lv_event_send(obj, LV_EVENT_SHORT_CLICKED, lvgl_indev_keyenc);
					lv_event_send(obj, LV_EVENT_CLICKED, lvgl_indev_keyenc);

					// DBG("group.send(KEY_ENTER)");
					lv_group_send_data(grp, LV_KEY_ENTER);
				}else{
					// DBG("grp.cnt < 2: clear_state(PRESSED)");
					lv_obj_clear_state(obj, LV_STATE_PRESSED);
				};
				break;
			};
			// DBG("group.set_edit(true)");
			lv_group_set_editing(grp, true);
			break;

		case LV_KEY_ESC:
			if(lv_group_get_editing(grp))
			{	
				// DBG("group.set_edit(false)");
				lv_group_set_editing(grp, false);
			}else{
				// DBG("group.send(ESC)");
				lv_group_send_data(grp, LV_KEY_ESC);
			};
			break;
		default:
			WARNING("Unknown key %d sent to TreeMenu", key);
			return false;
	};
	return true;
};
