#ifndef __SOOGH_COLOR_H
#define __SOOGH_COLOR_H

#include <LGFX_TFT_eSPI.hpp>

#define COLOR_BLACK			lv_color_black()
#define COLOR_GREY			lv_palette_main(LV_PALETTE_GREY)
#define COLOR_RED			lv_palette_main(LV_PALETTE_RED)
#define COLOR_ORANGE		lv_palette_main(LV_PALETTE_ORANGE)
#define COLOR_GREEN			lv_palette_main(LV_PALETTE_GREEN)

#define COLOR_DARK_GREEN	lv_palette_darken(LV_PALETTE_GREEN, 1)
#define COLOR_LIGHT_GREY	lv_palette_lighten(LV_PALETTE_GREY, 4)

#endif // __SOOGH_COLOR_H
