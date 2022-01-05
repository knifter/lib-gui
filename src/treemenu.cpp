#include "treemenu.h"

#include <soogh.h>

#include <tools-log.h>

#include <soogh-debug.h>
/*
Class hierarchy:
	MenuItem 			(drawable: draw_item, draw_open)
		SubMenu 		(actual menu)
			TreeMenu	(Top most Menu)
		FloatField 		(float editor)
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

bool MenuItem::handle(uint32_t key)
{
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
	if(_close_cb)
		_close_cb(this, _close_data);

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

/*** Separator ***************************************************************************************/
void MenuSeparator::draw_btn(lv_obj_t *lv_list)
{
	lv_list_add_text(lv_list, _text);
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

	// fake click on checkbox if click was on btn
	if(e->target != me->_sw)
	{
		lv_event_send(me->_sw, LV_EVENT_PRESSED, nullptr);
		lv_event_send(me->_sw, LV_EVENT_RELEASED, nullptr);
	};

	(*(me->value)) = lv_obj_has_state(me->_sw, LV_STATE_CHECKED);
};

/*** ActionItem ***************************************************************************************/
ActionField::ActionField(MenuItem *parent, const char *text, treemenu_cb_t *func, void* data) : MenuItem(parent, text)
{
	_change_cb = func; 
	_change_data = data;
};

void ActionField::draw_btn(lv_obj_t *lv_list)
{
	lv_obj_t *btn = lv_list_add_btn(lv_list, nullptr, _text);
	lv_obj_add_event_cb(btn, click_cb, LV_EVENT_CLICKED, this);

	root()->group_add(btn);
};
/* static */ void ActionField::click_cb(lv_event_t *e)
{
	ActionField* me = static_cast<ActionField*>(e->user_data);

	if(me->_change_cb)
		me->_change_cb(me, me->_change_data);
};

/*** FloatField ***************************************************************************************/
FloatField::FloatField(MenuItem *parent, const char *text, float *f, float min, float max) 
	: MenuItem(parent, text), value(f), min_value(min), max_value(max) 
{ 
	if(*value < min_value)
		min_value = *value;
	if(*value > max_value)
		max_value = *value;
};

void FloatField::draw_btn(lv_obj_t *lv_list)
{
	// TODO: can this be moved to MenuItem?
	_btn = lv_list_add_btn(lv_list, nullptr, _text);
	lv_obj_add_event_cb(_btn, btn_clicked_cb, LV_EVENT_CLICKED, this);
	lv_obj_set_flex_flow(_btn, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_style_pad_row(_btn, 3, 0);

	_btn_lbl = lv_label_create(_btn);
	lv_label_set_text_fmt(_btn_lbl, "%.02f", *value);
	// lv_obj_set_flex_grow(_btn_lbl, 1);
	lv_obj_set_style_text_color(_btn_lbl, COLOR_GREY, 0);

	root()->group_add(_btn);
}; 

/* static */ void FloatField::btn_clicked_cb(lv_event_t *e)
{
	FloatField* me = static_cast<FloatField*>(e->user_data);
	me->open();
};

int FloatField::digits()
{
	int min_digits = ceil(log10(min_value)) + 1; // +minus sign
	int max_digits = ceil(log10(max_value));
	// DBG("mindig = %d, maxdig = %d", min_digits, max_digits);
	return max(min_digits, max_digits) + decimals;
};

bool FloatField::handle(uint32_t key)
{
	DBG("key: %u", key);
	static bool edit = true;
	switch(key)
	{
		case LV_KEY_LEFT:
			if(edit)
				lv_spinbox_step_prev(_spinbox);
			else
				lv_spinbox_decrement(_spinbox);
			break;
		case LV_KEY_RIGHT:	
			if(edit)
				lv_spinbox_step_next(_spinbox); 
			else
				lv_spinbox_increment(_spinbox);
			break;
		case LV_KEY_ENTER:
			if(edit)
			{
				edit = false;
			}else{
				edit = true;
			};
			DBG("Edit = %d", edit);
			break;
		case LV_KEY_ESC:
			close();
			break;
	};
	export_value();

	return true;
};

void FloatField::export_value()
{
	if(!_spinbox)
		return;
	(*value) = lv_spinbox_get_value(_spinbox) / pow(10, decimals);
	lv_label_set_text_fmt(_btn_lbl, "%.02f", *value);
};

void FloatField::draw_open()
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
		lv_obj_set_pos(_spinbox, bpos.x1 - 10, bpos.y1 - 10);
		lv_obj_set_size(_spinbox, w + 20, h + 20);


		int digits = this->digits();
        int factor = pow(10, decimals);
		lv_spinbox_set_range(_spinbox, min_value*factor, max_value*factor);
		lv_spinbox_set_digit_format(_spinbox, digits, digits - decimals);
		lv_spinbox_set_value(_spinbox, (*value) * factor);
    	// DBG("min/max = %f/%f, val = %f, digs = %d, dec = %d, mult = %f", min_value, max_value, *value, digits, decimals, pow(10, decimals));

		_spinbox->user_data = this;
	    // lv_spinbox_t * sb = (lv_spinbox_t *) _spinbox;
		
		lv_group_add_obj(grp, _spinbox);
		// lv_obj_add_event_cb(_spinbox, sb_key_cb, LV_EVENT_KEY, this);
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

void FloatField::draw_close()
{
	lv_obj_del(_spinbox); 	_spinbox=nullptr;
#ifdef SOOGH_TOUCH
	lv_obj_del(_btns);		_btns = nullptr;
#endif

	// modify btn
	// lv_group_set_editing(root()->group_top(), false);
    lv_obj_clear_state(_btn, LV_STATE_CHECKED);
	// lv_obj_set_style_bg_color(_btn, COLOR_BLUE, 0);

	root()->group_pop();
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
	me->export_value();
};

/*** SubMenu ***************************************************************************************/
void SubMenu::draw_open()
{
	lv_obj_t *lv_parent = _btn;

    lv_img_set_src(_btn_img, LV_SYMBOL_DOWN);

	// The mnu is a list
	_list = lv_list_create(lv_parent);
	lv_obj_set_style_border_width(_list, 0, 0);
	lv_obj_set_height(_list, 45*_children.size());

	for(auto child: _children)
		child->draw_btn(_list);
};

void SubMenu::draw_close()
{
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

MenuSeparator* SubMenu::addSeparator(const char* text)
{
	return new MenuSeparator(this, text);
};

SubMenu* SubMenu::addSubMenu(const char* text)
{
	return new SubMenu(this, text);
};

FloatField* SubMenu::addFloat(const char* name, float* f, float min, float max)
{
	return new FloatField(this, name, f, min, max);
};

ActionField* SubMenu::addAction(const char* name, treemenu_cb_t *func, void *data)
{
	return new ActionField(this, name, func, data);
};

BooleanField* SubMenu::addSwitch(const char* name, bool *b)
{
	return new BooleanField(this, name, b, BooleanField::BOOLTYPE_SWITCH);
};

BooleanField* SubMenu::addCheckbox(const char* name, bool *b)
{
	return new BooleanField(this, name, b, BooleanField::BOOLTYPE_CHECKBOX);
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
	// Workaround: #2963
	// make indev forget its group, must be one of mine, which I'm sure is deleted now
	lv_indev_set_group(lvgl_indev_keyenc,  nullptr);
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

	// First (close) button
	lv_obj_t *btn = lv_list_add_btn(_list, LV_SYMBOL_CLOSE, "Close");
	lv_obj_add_event_cb(btn, TreeMenu::close_cb, LV_EVENT_CLICKED, this);
	group_add(btn);

	for(auto child: _children)
		child->draw_btn(_list);
};

void TreeMenu::draw_close()
{
	lv_obj_del(_list); _list = nullptr;

	group_pop();
};

/*static*/ void TreeMenu::close_cb(lv_event_t *e)
{
	TreeMenu* me = static_cast<TreeMenu*>(e->user_data);
	me->close();
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

	// Workaround: #2963
	if(_grpstack.empty())
	{
		// make indev forget its group, must be one of mine, which I'm sure is deleted now
		lv_indev_set_group(lvgl_indev_keyenc,  nullptr);
	}else{
		lv_indev_set_group(lvgl_indev_keyenc,  _grpstack.top());
	};
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

void TreeMenu::sendKey(menukey_t key)
{
	if(_grpstack.empty())
	{
		WARNING("No group to send key to.");
		return;
	};
	lv_group_t* grp = _grpstack.top();

	bool editable_or_scrollable = true;
	lv_obj_t *obj = lv_group_get_focused(grp);
	if(!obj)
	{
		WARNING("No obj focussed");
		return;
	};

	// See if the item owning the object wants to handle the event
	if(obj->user_data)
	{
		DBG("Object has user-data.");
		MenuItem* item = static_cast<MenuItem*>(obj->user_data);
		if(item->handle(key))
		{
			DBG("key-event handled");
			return;
		};
	};

	// simulated indev mode
	editable_or_scrollable = lv_obj_is_editable(obj) || lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
	// DBG("edit_or_scrollable = %s, group.editing = %s", editable_or_scrollable ? "true": "false", lv_group_get_editing(grp) ? "true":"false");
	switch(key)
	{
		case KEY_NONE:
			DBG("No key given.");
			break;

		case KEY_LEFT:
			if(lv_group_get_editing(grp))
			{
				// DBG("group.send(LEFT)");
				lv_group_send_data(grp, LV_KEY_LEFT);
			}else{
				// DBG("group.prev");
				lv_group_focus_prev(grp);
			};
			break;

		case KEY_RIGHT:
			if(lv_group_get_editing(grp))
			{
				// DBG("group.send(RIGHT)");
				lv_group_send_data(grp, LV_KEY_RIGHT);
			}else{
				// DBG("group.next");
				lv_group_focus_next(grp);
			};
			break;

		case KEY_ENTER:
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

		case KEY_ESC:
		{
			if(lv_group_get_editing(grp))
			{	
				// DBG("group.set_edit(false)");
				lv_group_set_editing(grp, false);
			}else{
				// DBG("group.send(ESC)");
				lv_group_send_data(grp, LV_KEY_ESC);
			};
			break;
		};
	};
};
