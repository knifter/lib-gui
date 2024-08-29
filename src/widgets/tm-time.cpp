#if 0
#include "widgets/treemenu.h"

#include "tools-log.h"
#include "tools-debug.h"

#include "soogh-color.h"
#include "soogh-conf.h"
#include "soogh-debug.h"


TimeField::TimeField(MenuItem *parent, const char *name, struct tm* target) 
	: MenuItem(parent, name), _target(target)
{
};

void TimeField::draw_btn(lv_obj_t *lv_list)
{
	// TODO: can this be moved to MenuItem?
	_btn = lv_list_add_btn(lv_list, nullptr, _text);
	lv_obj_add_event_cb(_btn, btn_click_cb, LV_EVENT_CLICKED, this);
	lv_obj_set_flex_flow(_btn, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_style_pad_row(_btn, 3, 0);

	_btn_lbl = lv_label_create(_btn);
	lv_label_set_text_fmt(_btn_lbl, "%02d-%02d", _target->tm_hour, _target->tm_min);

	// lv_obj_set_flex_grow(_btn_lbl, 1);
	lv_obj_set_style_text_color(_btn_lbl, COLOR_GREY, 0);

	root()->group_add(_btn);
};

/* static */ void TimeField::btn_click_cb(lv_event_t *e)
{
	TimeField* me = static_cast<TimeField*>(e->user_data);
	me->open();
};

void TimeField::draw_open()
{
	lv_group_t* grp = root()->group_push();
	// DBG_REG_PTR(grp, "TimeField.grp");

	// get coords of label
	// lv_area_t bpos;
	// lv_obj_get_coords(_btn_lbl, &bpos);

	// draw (floating) date selector right over label
	// const int total_w = bpos.x2 - bpos.x1 + 20;
	// const int total_h = bpos.y2 - bpos.y1 + 12;
	

	lv_group_set_editing(grp, true);
	
#ifdef SOOGH_TOUCH
#endif // SOOGH_TOUCH
};

bool TimeField::handleKey(lv_key_t key, lv_obj_t* obj)
{
	static time_t last_enter = 0;
	time_t now = millis();
	DBG("obj = %s, key %d", DBG_PTR_NAME(obj), key);
	
	switch(key)
	{
		case LV_KEY_LEFT:
			// if(_edit)
			// else
			break;
		case LV_KEY_RIGHT:	
			// if(_edit)
			// else
			break;
		case LV_KEY_ENTER:
			// detect double click to close
			if((now - last_enter) < 500)
			{
				close();
				return true;
			};
			last_enter = now;

			// days
			// export_selected_value();

			_edit = !_edit;
			// lv_obj_set_style_bg_color(_spinbox, _edit ? COLOR_RED : COLOR_BLUE, LV_PART_CURSOR);
			break;
		case LV_KEY_ESC:
			close();
			break;
	};

	return false;
};

void TimeField::export_value()
{

	// Update button label
	lv_label_set_text_fmt(_btn_lbl, "%02d-%02d", _target->tm_hour, _target->tm_min);


	// propagate to user
	call_onchange();
};

void TimeField::draw_close()
{
	// lv_obj_del(_spinbox); 	_spinbox=nullptr;

#ifdef SOOGH_TOUCH
#endif

	root()->group_pop();
};

#endif