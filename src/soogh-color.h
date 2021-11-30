#ifndef __SOOGH_COLOR_H
#define __SOOGH_COLOR_H

#include "soogh-lgfx.h"

#define COLOR_BLACK			lv_color_black()
#define COLOR_WHITE			lv_color_white()
#define COLOR_GREY			lv_palette_main(LV_PALETTE_GREY)
#define COLOR_RED			lv_palette_main(LV_PALETTE_RED)
#define COLOR_ORANGE		lv_palette_main(LV_PALETTE_ORANGE)
#define COLOR_GREEN			lv_palette_main(LV_PALETTE_GREEN)
#define COLOR_PURPLE		lv_palette_main(LV_PALETTE_PURPLE)

#define COLOR_DARK_GREEN	lv_palette_darken(LV_PALETTE_GREEN, 2)
#define COLOR_LIGHT_RED		lv_palette_lighten(LV_PALETTE_RED, 2)
#define COLOR_LIGHT_GREEN	lv_palette_lighten(LV_PALETTE_GREEN, 2)
#define COLOR_LIGHT_GREY	lv_palette_lighten(LV_PALETTE_GREY, 4)
#define COLOR_LIGHT_PURPLE	lv_palette_lighten(LV_PALETTE_PURPLE, 3)

#endif // __SOOGH_COLOR_H
