#ifndef SCREENS_H
#define SCREENS_H

#include "controller.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "lvgl.h"
#include "manual_mode.h"
#include "menu.h"

typedef enum {
    SCREEN_NONE,
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
    controller_data_t controller_data;
    QueueHandle_t incoming_queue_commands;
    QueueHandle_t outgoing_queue_lcd;

    lv_obj_t *main_screen;
    screen_t current_screen;
    nav_t current_nav;

    menu_obj_t menu_obj;
    manual_mode_obj_t manual_mode_obj;
} s_screen_manager_t;

typedef struct s_screen_manager_t *screen_manager_t;

screen_manager_t screen_manager_init(controller_data_t controller_data, QueueHandle_t incoming_queue_commands, QueueHandle_t outgoing_queue_lcd);
void screen_manager_update(screen_manager_t screen_manager, controller_data_t controller_data);

void screen_manager_go_back(screen_manager_t screen_manager);
void screen_manager_set_screen(screen_manager_t screen_manager, screen_t screen);

#endif
