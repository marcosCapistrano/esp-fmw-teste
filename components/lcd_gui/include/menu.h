#ifndef SCREEN_MENU_H
#define SCREEN_MENU_H

#include "lvgl.h"

typedef struct s_btn_manual_t {
	lv_style_t *title_style;
	lv_style_t *subtitle_style;
} s_btn_manual_t;

typedef struct s_btn_manual_t *btn_manual_t;

void screen_menu_init(lv_obj_t* main_screen);

#endif