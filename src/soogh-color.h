#ifndef __SOOGH_COLOR_H
#define __SOOGH_COLOR_H

#include "soogh-lgfx.h"


#define COLOR_BLACK			lv_color_black()
#define COLOR_WHITE			lv_color_white()

#define COLOR_RED			lv_palette_main(LV_PALETTE_RED)
#define COLOR_PINK			lv_palette_main(LV_PALETTE_PINK)
#define COLOR_PURPLE		lv_palette_main(LV_PALETTE_PURPLE)
#define COLOR_DEEP_PURPLE	lv_palette_main(LV_PALETTE_DEEP_PURPLE)
#define COLOR_INDIGO		lv_palette_main(LV_PALETTE_INDIGO)
#define COLOR_BLUE			lv_palette_main(LV_PALETTE_BLUE)
#define COLOR_LIGHT_BLUE	lv_palette_main(LV_PALETTE_LIGHT_BLUE)
#define COLOR_CYAN			lv_palette_main(LV_PALETTE_CYAN)
#define COLOR_TEAL			lv_palette_main(LV_PALETTE_TEAL)
#define COLOR_GREEN			lv_palette_main(LV_PALETTE_GREEN)
#define COLOR_LIGHT_GREEN	lv_palette_main(LV_PALETTE_LIGHT_GREEN)
#define COLOR_LIME			lv_palette_main(LV_PALETTE_LIME)
#define COLOR_YELLOW		lv_palette_main(LV_PALETTE_YELLOW)
#define COLOR_AMBER			lv_palette_main(LV_PALETTE_AMBER)
#define COLOR_ORANGE		lv_palette_main(LV_PALETTE_ORANGE)
#define COLOR_DEEP_ORANGE	lv_palette_main(LV_PALETTE_DEEP_ORANGE)
#define COLOR_BROWN			lv_palette_main(LV_PALETTE_BROWN)
#define COLOR_BLUE_GREY		lv_palette_main(LV_PALETTE_BLUE_GREY)
#define COLOR_GREY			lv_palette_main(LV_PALETTE_GREY)

#define COLOR_GREEN_DARK2	lv_palette_darken(LV_PALETTE_GREEN, 2)
#define COLOR_GREEN_LIGHT2	lv_palette_lighten(LV_PALETTE_GREEN, 2)
#define COLOR_RED_LIGHT2	lv_palette_lighten(LV_PALETTE_RED, 2)
#define COLOR_PURPLE_LIGHT3	lv_palette_lighten(LV_PALETTE_PURPLE, 3)

#define COLOR_GREY_DARK1	lv_palette_darken(LV_PALETTE_GREY, 1)
#define COLOR_GREY_DARK2	lv_palette_darken(LV_PALETTE_GREY, 2)
#define COLOR_GREY_DARK3	lv_palette_darken(LV_PALETTE_GREY, 3)
#define COLOR_GREY_DARK4	lv_palette_darken(LV_PALETTE_GREY, 4)
#define COLOR_GREY_LIGHT1	lv_palette_lighten(LV_PALETTE_GREY, 1)
#define COLOR_GREY_LIGHT2	lv_palette_lighten(LV_PALETTE_GREY, 2)
#define COLOR_GREY_LIGHT3	lv_palette_lighten(LV_PALETTE_GREY, 3)
#define COLOR_GREY_LIGHT4	lv_palette_lighten(LV_PALETTE_GREY, 4)

#endif // __SOOGH_COLOR_H
