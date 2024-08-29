#if 0

#include "treemenu.h"

#include "tools-log.h"
#include "tools-debug.h"

#include "soogh-color.h"
#include "soogh-conf.h"
#include "soogh-debug.h"


DateField::DateField(MenuItem *parent, const char *name, struct tm* target) 
	: MenuItem(parent, name), _target(target)
{
};

void DateField::draw_btn(lv_obj_t *lv_list)
{
	// TODO: can this be moved to MenuItem?
	_btn = lv_list_add_btn(lv_list, nullptr, _text);
	lv_obj_add_event_cb(_btn, btn_click_cb, LV_EVENT_CLICKED, this);
	lv_obj_set_flex_flow(_btn, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_style_pad_row(_btn, 3, 0);

	_btn_lbl = lv_label_create(_btn);
	lv_label_set_text_fmt(_btn_lbl, "%4d-%02d-%02d", _target->tm_year, _target->tm_mon, _target->tm_mday);

	// lv_obj_set_flex_grow(_btn_lbl, 1);
	lv_obj_set_style_text_color(_btn_lbl, COLOR_GREY, 0);

	root()->group_add(_btn);
};

/* static */ void DateField::btn_click_cb(lv_event_t *e)
{
	DateField* me = static_cast<DateField*>(e->user_data);
	me->open();
};

void DateField::draw_open()
{
	// get coords of label
	// lv_area_t bpos;
	// lv_obj_get_coords(_btn_lbl, &bpos);

	// draw (floating) date selector right over label
	// const int total_w = bpos.x2 - bpos.x1 + 20;
	// const int total_h = bpos.y2 - bpos.y1 + 12;

	DBG_PTR_REGISTER(this, "DateField(me)");
	_calendar = lv_calendar_create(lv_layer_top());
	{
		lv_obj_set_size(_calendar, DISPLAY_WIDTH, DISPLAY_HEIGHT);
		// lv_obj_align(_calendar, LV_ALIGN_CENTER, 0, 0);
		// lv_obj_add_event_cb(calendar, event_handler, LV_EVENT_ALL, NULL);
		// lv_calendar_header_dropdown_create(_calendar);
		_calendar_head = lv_calendar_header_arrow_create(_calendar);

		lv_calendar_set_today_date(_calendar, (uint32_t) _target->tm_year, _target->tm_mon, _target->tm_mday);
		lv_calendar_set_showed_date(_calendar, _target->tm_year, _target->tm_mon);

	    lv_obj_add_event_cb(_calendar, calender_event_cb, LV_EVENT_ALL, this);

		// set a ref to this class to make MenuItem call sendKey()
		_calendar->user_data = this;
	};
	DBG_PTR_REGISTER(_calendar, "_calendar");
	
	lv_group_t* grp = root()->group_push();
	// DBG_REG_PTR(grp, "DateField.grp");

	// focus days
	lv_obj_t* btnm = lv_calendar_get_btnmatrix(_calendar);
	lv_group_add_obj(grp, btnm);
	lv_group_focus_obj(btnm);
	btnm->user_data = this;
	DBG_PTR_REGISTER(btnm, "btnm");

	// focus year/month
	lv_group_add_obj(grp, _calendar_head);
	lv_group_focus_obj(_calendar_head);
	DBG_PTR_REGISTER(_calendar_head, "_calendar_head");

	lv_group_set_editing(grp, true);
	
#ifdef SOOGH_TOUCH
#endif // SOOGH_TOUCH
};

/* static */ void DateField::calender_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    // lv_obj_t * obj = lv_event_get_current_target(e);
	DateField* me = static_cast<DateField*>(e->user_data);

	debug_event_input(e);

    if(code == LV_EVENT_VALUE_CHANGED) 
	{
		DBG("VALUECHANGED");
	};
	if(code == LV_EVENT_PRESSED)
	{
		DBG("DateField.PRESSED = %p", me);
		me->export_selected_value();
		me->close();
    };
};

bool DateField::handleKey(lv_key_t key, lv_obj_t* obj)
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

void DateField::export_selected_value()
{
	if(!_calendar)
		return;

	// Get date from cal and make it current date
	lv_calendar_date_t selected;
	if(!lv_calendar_get_pressed_date(_calendar, &selected))
		return;
	lv_calendar_set_today_date(_calendar, selected.year, selected.month, selected.day);
	// lv_calendar_set_today_date(_calendar, _target->tm_year, _target->tm_mon, _target->tm_mday);

	// copy it to target 
	_target->tm_year = selected.year;
	_target->tm_mon  = selected.month;
	_target->tm_mday = selected.day;

	// Update button label
	lv_label_set_text_fmt(_btn_lbl, "%4d-%02d-%02d", _target->tm_year, _target->tm_mon, _target->tm_mday);


	// propagate to user
	call_onchange();
};

void DateField::draw_close()
{
	// lv_obj_del(_spinbox); 	_spinbox=nullptr;
	lv_obj_del(_calendar); 	_calendar=nullptr;

#ifdef SOOGH_TOUCH
#endif

	root()->group_pop();
};

#endif
