#ifndef SCREEN_MENU_H
#define SCREEN_MENU_H

#include "lvgl.h"

typedef struct s_btn_menu_small_t {
	lv_obj_t *btn;
	lv_style_t *title_style;
	lv_style_t *subtitle_style;
} s_btn_menu_small_t;

typedef struct s_btn_menu_small_t *btn_menu_small_t;

typedef struct s_btn_menu_big_t {
	lv_obj_t *btn;
	lv_style_t *title_style;
} s_btn_menu_big_t;

typedef struct s_btn_menu_big_t *btn_menu_big_t;

void screen_menu_init(lv_obj_t* main_screen);

#endif