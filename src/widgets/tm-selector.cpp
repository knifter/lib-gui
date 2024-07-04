#include "treemenu.h"

#include "soogh-color.h"
#include "soogh-conf.h"

#ifdef DEBUG
	#include "tools-log.h"
#endif


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

bool SelectorField::handleKey(lv_key_t key, lv_obj_t*)
{
	// user_data had to be set but we don't handle keys. But also do not propagate to parent.
	// FIXME: false == not handles -> propagate to parent?
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
		// ERROR("target id not found after select. Serious bug.");
		*_target = _items[0].id;
	};
#endif

	lv_label_set_text(_btn_lbl, item->shortname);

	root()->group_pop();
};

