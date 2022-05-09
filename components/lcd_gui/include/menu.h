#ifndef SCREEN_MENU_H
#define SCREEN_MENU_H

#include "lvgl.h"
// #include "screen.h"

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

typedef struct s_menu_obj_t {
	btn_menu_small_t btn_menu_small[2];
	btn_menu_big_t btn_menu_big;
} s_menu_obj_t;

typedef struct s_menu_obj_t *menu_obj_t;

typedef struct s_screen_manager_t *screen_manager_t;

menu_obj_t screen_menu_init(lv_obj_t* main_screen, screen_manager_t screen_manager);
void free_screen_menu(menu_obj_t menu_obj);

#endif