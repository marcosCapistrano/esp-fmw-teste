#ifndef SCREEN_MENU_H
#define SCREEN_MENU_H

#include "lvgl.h"
#include "screen_manager.h"
#include "screen.h"

typedef struct s_screen_menu_t *screen_menu_t;
typedef struct {} s_screen_menu_t;

screen_i menu_as_screen;

/*
	Forward refs
*/
typedef struct s_screen_manager_t *screen_manager_t;

screen_menu_t screen_menu_create(lv_obj_t *main_screen, screen_manager_t screen_manager);

static void btn_menu_small_create(lv_obj_t *parent, int x, int y, const char *subtitle, int subtitle_x, int subtitle_y, void *event_handler, screen_manager_t screen_manager);
static void btn_menu_big_create(lv_obj_t *parent);
static void btn_manual_event_handler(lv_event_t *e);
#endif