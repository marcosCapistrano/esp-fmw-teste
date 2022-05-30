#ifndef SCREEN_MANUAL_H
#define SCREEN_MANUAL_H

#include "lvgl.h"
#include "screen.h"
#include "screen_manager.h"
#include "content_manager.h"

typedef struct s_screen_manual_t *screen_manual_t;
typedef struct s_screen_manual_t {
    lv_obj_t *header;
    content_manager_t content_manager;
} s_screen_manual_t;

screen_i manual_as_screen;

/*
        Forward refs
*/
typedef struct s_screen_manager_t *screen_manager_t;

screen_manual_t screen_manual_create(lv_obj_t *main_screen, screen_manager_t screen_manager);

#endif