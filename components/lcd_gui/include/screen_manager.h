#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "controller.h"
#include "common_controller.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "lvgl.h"
#include "menu.h"
#include "screen.h"

typedef struct s_nav_t *nav_t;
typedef struct s_nav_t {
    screens_t screen;
    nav_t next;
} s_nav_t;

typedef struct s_screen_manager_t *screen_manager_t;
typedef struct s_screen_manager_t {
    controller_data_t controller_data;
    QueueHandle_t incoming_queue_commands;
    QueueHandle_t outgoing_queue_lcd;

    void (*set_screen_cb)(screens_t screen_type);
    lv_obj_t *main_screen;
    screen_t current_screen;
    nav_t navigation;
} s_screen_manager_t;

screen_manager_t screen_manager_init(controller_data_t controller_data, QueueHandle_t incoming_queue_commands, QueueHandle_t outgoing_queue_lcd);
void screen_manager_update(screen_manager_t screen_manager, controller_event_t incoming_event);
void screen_manager_set_screen(screen_manager_t screen_manager, screens_t screen_type);
void screen_manager_go_back(screen_manager_t screen_manager);

static nav_t nav_init();
static void nav_push(nav_t *nav, screens_t screen);
static void nav_pop(nav_t *nav);

#endif
