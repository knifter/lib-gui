#include "treemenu.h"

#include "soogh-color.h"
#include <Arduino.h>
#include <math.h>

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

bool NumberField::handleKey(lv_key_t key, lv_obj_t*)
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

		// set a ref to this class to make MenuItem call sendKey()
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
