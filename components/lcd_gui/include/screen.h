#ifndef SCREENS_H
#define SCREENS_H

#include "lvgl.h"
#include "menu.h"
#include "manual_mode.h"
#include "controller.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

typedef enum {
    MENU,
    MANUAL_MODE,
    AUTO_MODE,
} screen_t;

typedef struct s_nav_t *nav_t;

typedef struct s_nav_t {
    screen_t screen;
    nav_t next;
} s_nav_t;

typedef struct s_manual_mode_obj_t *manual_mode_obj_t;

typedef struct s_screen_manager_t {
    QueueHandle_t input_control_queue;
    QueueHandle_t output_notify_queue;
    controller_t controller;

    lv_obj_t *main_screen;
    screen_t current_screen;
    nav_t current_nav;

    menu_obj_t menu_obj; 
    manual_mode_obj_t manual_mode_obj;
} s_screen_manager_t;

screen_manager_t screen_manager_init(lv_obj_t *main_screen, QueueHandle_t input_control_queue, QueueHandle_t output_notify_queue, controller_t controller);
void screen_manager_go_back(screen_manager_t screen_manager);
void screen_manager_set_screen(screen_manager_t screen_manager, screen_t screen);

#endif
