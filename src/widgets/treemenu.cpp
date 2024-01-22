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

bool MenuItem::sendKey(lv_key_t key)
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

/*** BooleanField ***************************************************************************************/
void BooleanField::draw_btn(lv_obj_t *lv_list)
{
	lv_obj_t *btn = lv_list_add_btn(lv_list, nullptr, _text);
	lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_style_pad_row(btn, 3, 0);

	switch(_type)
	{
		case BOOLTYPE_CHECKBOX:  
			_sw = lv_checkbox_create(btn); 
			lv_checkbox_set_text_static(_sw, ""); 
			break;
		case BOOLTYPE_SWITCH:	 _sw = lv_switch_create(btn); break;
		default: return;
	};
	
	// lv_obj_set_flex_grow(_sw, 1);
	// lv_obj_set_style_min_height(_sw, 0, 0);
	lv_obj_set_size(_sw, 40, 20);

	if(*value == true)
    	lv_obj_add_state(_sw, LV_STATE_CHECKED);

	lv_obj_add_event_cb(_sw, click_cb, LV_EVENT_CLICKED, this);
	lv_obj_add_event_cb(btn, click_cb, LV_EVENT_CLICKED, this);

	root()->group_add(btn);
};
/* static */ void BooleanField::click_cb(lv_event_t *e)
{
	BooleanField* me = static_cast<BooleanField*>(e->user_data);

	bool old_val = *(me->value);

	// fake click on checkbox if click was on btn
	if(e->target != me->_sw)
	{
		lv_event_send(me->_sw, LV_EVENT_PRESSED, nullptr);
		lv_event_send(me->_sw, LV_EVENT_RELEASED, nullptr);
	};

	bool new_value = lv_obj_has_state(me->_sw, LV_STATE_CHECKED);

	(*(me->value)) = new_value;

	if(old_val != new_value)
	{
	    me->call_onchange();
	}
};

/*** ActionItem ***************************************************************************************/
ActionField::ActionField(MenuItem *parent, const char *name, treemenu_cb_t *func, void* data, const void* lv_icon) : MenuItem(parent, name)
{
	_change_cb = func; 
	_change_data = data;
	_lv_icon = lv_icon;
};

void ActionField::draw_btn(lv_obj_t *lv_list)
{
	lv_obj_t *btn = lv_list_add_btn(lv_list, _lv_icon, _text);
	lv_obj_add_event_cb(btn, click_cb, LV_EVENT_CLICKED, this);

	root()->group_add(btn);
};
/* static */ void ActionField::click_cb(lv_event_t *e)
{
	ActionField* me = static_cast<ActionField*>(e->user_data);

	if(me->_change_cb)
		me->_change_cb(me, me->_change_data);
};

/*** NumberField ***************************************************************************************/
NumberField::NumberField(MenuItem *parent, const char *name, double *f, double min, double max) 
	: MenuItem(parent, name), value(f), min_value(min), max_value(max) 
{
};

void NumberField::draw_btn(lv_obj_t *lv_list)
{
	// TODO: can this be moved to MenuItem?
	_btn = lv_list_add_btn(lv_list, nullptr, _text);
	lv_obj_add_event_cb(_btn, btn_clicked_cb, LV_EVENT_CLICKED, this);
	lv_obj_set_flex_flow(_btn, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_style_pad_row(_btn, 3, 0);

	_btn_lbl = lv_label_create(_btn);
	lv_label_set_text_fmt(_btn_lbl, "%.*f", decimals, *value);
	// lv_obj_set_flex_grow(_btn_lbl, 1);
	lv_obj_set_style_text_color(_btn_lbl, COLOR_GREY, 0);

	root()->group_add(_btn);
}; 

/* static */ void NumberField::btn_clicked_cb(lv_event_t *e)
{
	NumberField* me = static_cast<NumberField*>(e->user_data);
	me->open();
};

bool NumberField::sendKey(lv_key_t key)
{
	static time_t last_enter = 0;
	time_t now = millis();
	switch(key)
	{
		case LV_KEY_LEFT:
			if(_edit)
				lv_spinbox_decrement(_spinbox);
			else
				lv_spinbox_step_prev(_spinbox);
			break;
		case LV_KEY_RIGHT:	
			if(_edit)
				lv_spinbox_increment(_spinbox);
			else
				lv_spinbox_step_next(_spinbox); 
			break;
		case LV_KEY_ENTER:
			// detect double click to close
			if((now - last_enter) < 500)
			{
				close();
				return true;
			};
			last_enter = now;

			_edit = !_edit;
			lv_obj_set_style_bg_color(_spinbox, _edit ? COLOR_RED : COLOR_BLUE, LV_PART_CURSOR);
			break;
		case LV_KEY_ESC:
			close();
			break;
	};
	export_value();

	return true;
};

void NumberField::export_value()
{
	if(!_spinbox)
		return;
	(*value) = lv_spinbox_get_value(_spinbox) / pow(10, decimals);
	lv_label_set_text_fmt(_btn_lbl, "%.*f", decimals, *value);
	call_onchange();
};

int NumberField::digits()
{
	// DBG("min/max: %f/%f", _min, _max);
	int min_digits = (min_value != 0) ? ceil(log10(abs(min_value)+1)) : 1;
	int max_digits = (max_value != 0) ? ceil(log10(abs(max_value)+1)) : 1;
	// DBG("mindig = %d, maxdig = %d", min_digits, max_digits);
	return max(min_digits, max_digits) + decimals;
};

void NumberField::draw_open()
{
	// modify btn
	// lv_obj_set_style_bg_grad_color(_btn, COLOR_RED_LIGHT, 0);
    lv_obj_add_state(_btn, LV_STATE_CHECKED);

	lv_group_t* grp = root()->group_push();

	// get coords of label
	lv_area_t bpos;
	lv_obj_get_coords(_btn_lbl, &bpos);


	// draw (floating) spinbox right over label
	_spinbox = lv_spinbox_create(lv_layer_top());
	{
		const int w = bpos.x2 - bpos.x1;
		const int h = bpos.y2 - bpos.y1;
		lv_obj_set_pos(_spinbox, bpos.x1 - 12, bpos.y1 - 12);
		lv_obj_set_size(_spinbox, w + 24, h + 24);

		int digits = this->digits();
        int factor = pow(10, decimals);

		// Start editing at the integer digit if not yet openend
		_edit = true;
		if(_lastpos == 0xFF)
			_lastpos = decimals;

		lv_spinbox_set_range(_spinbox, min_value*factor, max_value*factor);
		lv_spinbox_set_digit_format(_spinbox, digits, digits - decimals);
		lv_spinbox_set_value(_spinbox, (*value) * factor);
		lv_spinbox_set_pos(_spinbox, _lastpos);
		lv_obj_set_style_bg_color(_spinbox, _edit ? COLOR_RED : COLOR_BLUE, LV_PART_CURSOR);

    	// DBG("min/max = %f/%f, val = %f, digs = %d, dec = %d, mult = %f", _min, _max, *value, digits, _decimals, pow(10, _decimals));

		_spinbox->user_data = this;
		
		lv_group_add_obj(grp, _spinbox);
	};

	lv_group_focus_obj(_spinbox);
	lv_group_set_editing(grp, true);

#ifdef SOOGH_TOUCH
	// And floating buttons just below the spinbox
	_btns = lv_btnmatrix_create(lv_layer_top());
	{
		lv_obj_set_size(_btns, LV_PCT(80), 50);
		lv_obj_set_style_pad_all(_btns, 3, 0);

		static const char * map[] = {
			LV_SYMBOL_LEFT, LV_SYMBOL_MINUS, LV_SYMBOL_OK, LV_SYMBOL_PLUS, LV_SYMBOL_RIGHT, "" };
		lv_btnmatrix_set_map(_btns, map);
		lv_btnmatrix_set_btn_width(_btns, 2, 2);

		if(bpos.y1 < DISPLAY_HEIGHT /2)
		{
			// place below spinbox
			lv_obj_align_to(_btns, _spinbox, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
		}else{
			// place above spinbox
			lv_obj_align_to(_btns, _spinbox, LV_ALIGN_OUT_TOP_MID, 0, 0);
		};

		lv_obj_add_event_cb(_btns, btns_cb, LV_EVENT_VALUE_CHANGED, this);
	};
#endif // SOOGH_TOUCH
};

void NumberField::draw_close()
{
	// store cursor position
	_lastpos = log10( lv_spinbox_get_step(_spinbox) );

	lv_obj_del(_spinbox); 	_spinbox=nullptr;
#ifdef SOOGH_TOUCH
	lv_obj_del(_btns);		_btns = nullptr;
#endif

	// modify btn
    lv_obj_clear_state(_btn, LV_STATE_CHECKED);

	root()->group_pop();
};

#ifdef SOOGH_TOUCH
/* static */ void NumberField::btns_cb(lv_event_t * e)
{
	// lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * obj = lv_event_get_target(e);
	uint32_t id = lv_btnmatrix_get_selected_btn(obj);
	NumberField* me = static_cast<NumberField*>(e->user_data);
	switch(id)
	{
		case 0: lv_spinbox_step_prev(me->_spinbox); break;
		case 1: lv_spinbox_decrement(me->_spinbox); break;
		case 2: DBG("Close"); me->close(); break;
		case 3: lv_spinbox_increment(me->_spinbox); break;
		case 4: lv_spinbox_step_next(me->_spinbox); break;
		default: DBG("ID = %d", id); break;
	};
	me->export_value();
};
#endif // SOOGH_TOUCH

/*** SelectorField *********************************************************************************/
// SelectorField::SelectorField(MenuItem *parent, const char *name, uint32_t* target, selectionlist_t list)
SelectorField::SelectorField(MenuItem *parent, const char *name, int32_t* target, const item_t *items)
	: MenuItem(parent, name), _target(target), _items(items)
{
};

void SelectorField::draw_btn(lv_obj_t *lv_list)
{
	// TODO: can this be moved to MenuItem?
	_btn = lv_list_add_btn(lv_list, nullptr, _text);
	lv_obj_add_event_cb(_btn, btn_click_cb, LV_EVENT_CLICKED, this);
	lv_obj_set_flex_flow(_btn, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_style_pad_row(_btn, 3, 0);

	_btn_lbl = lv_label_create(_btn);
	// lv_label_set_text_fmt(_btn_lbl, "lbl");
	lv_obj_set_style_text_color(_btn_lbl, COLOR_GREY, 0);

	// get the shorttext by finding the currently set id
	const item_t *item = _items;
	while(item->shortname)
	{
		if(item->id == *_target)
			break;
		item++;
	};

	// if _target is not found, _target will be set to the last id == 0
	*_target = item->id;
    lv_label_set_text(_btn_lbl, item->shortname ? item->shortname : "-");

	root()->group_add(_btn);
};
/* static */ void SelectorField::btn_click_cb(lv_event_t *e)
{
	SelectorField* me = static_cast<SelectorField*>(e->user_data);
	me->open();
};

void SelectorField::draw_open()
{
    // lv_obj_add_state(_btn, LV_STATE_CHECKED);

	lv_group_t* grp = root()->group_push();

	// get coords of label
	lv_area_t bpos;
	lv_obj_get_coords(_btn_lbl, &bpos);


	// draw (floating) spinbox right over label
	_list = lv_list_create(lv_layer_top());
	{
		const int w = DISPLAY_WIDTH - bpos.x1;
		const int h = DISPLAY_HEIGHT - bpos.y1;
		lv_obj_set_pos(_list, bpos.x1 - 12, bpos.y1 - 12);
		lv_obj_set_size(_list, w + 24, h + 24);

		const item_t *item = _items;
		while(item->shortname)
		{
			// create button
			lv_obj_t *btn = lv_list_add_btn(_list, nullptr, item->longname ? item->longname : item->shortname);
			btn->user_data = this;			
			lv_obj_add_event_cb(btn, choose_click_cb, LV_EVENT_CLICKED, const_cast<item_t*>(item));
			lv_group_add_obj(grp, btn);

			// set to currently selected item
			if(item->id == *_target)
				lv_group_focus_obj(btn);

			item++;
		};
	};
};
/* static */ void SelectorField::choose_click_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
	SelectorField* me = static_cast<SelectorField*>(btn->user_data);
	const item_t* item = static_cast<const item_t*>(e->user_data);

	uint32_t prv_id = *(me->_target);
	*(me->_target) = item->id;
	if(prv_id != item->id)
	{
		me->call_onchange();
	};

	me->close();
};

bool SelectorField::sendKey(lv_key_t key)
{
	// user_data had to be set but we don't handle keys. But also do not propagate to parent.
	return false;
};

void SelectorField::draw_close()
{
	lv_obj_del(_list); _list = nullptr;

	// lv_obj_clear_state(_btn, LV_STATE_CHECKED);

	const item_t *item = _items;
	bool found = false;
	while(item->shortname)
	{
		if(item->id == *_target)
		{
			found = true;
			break;
		};
		item++;
	};

#ifdef DEBUG
	// This should never happen
	if(!found)
	{
		ERROR("target id not found after select. Serious bug.");
		*_target = _items[0].id;
	};
#endif

	lv_label_set_text(_btn_lbl, item->shortname);

	root()->group_pop();
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
		if(item->sendKey(key))
		{
			return true;
		};
	};

	// simulated indev mode
	editable_or_scrollable = lv_obj_is_editable(obj) || lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
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
