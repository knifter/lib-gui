#include "treemenu.h"

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
