#include "treemenu.h"

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
