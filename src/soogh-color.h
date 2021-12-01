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

#define COLOR_GREEN_DARK	lv_palette_darken(LV_PALETTE_GREEN, 2)
#define COLOR_RED_LIGHT		lv_palette_lighten(LV_PALETTE_RED, 2)
#define COLOR_GREEN_LIGHT	lv_palette_lighten(LV_PALETTE_GREEN, 2)
#define COLOR_GREY_LIGHT	lv_palette_lighten(LV_PALETTE_GREY, 4)
#define COLOR_PURPLE_LIGHT	lv_palette_lighten(LV_PALETTE_PURPLE, 3)

#endif // __SOOGH_COLOR_H
