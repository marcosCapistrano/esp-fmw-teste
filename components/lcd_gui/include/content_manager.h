#ifndef CONTENT_MANAGER_H
#define CONTENT_MANAGER_H

#include "lvgl.h"
#include "screen_manager.h"
#include "content.h"
#include "common_controller.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"


typedef struct s_btn_stage_t *btn_stage_t;
typedef struct s_btn_stage_t {
    lv_obj_t *label;

    controller_stage_t value;
} s_btn_stage_t;

typedef struct s_content_manager_t *content_manager_t;
typedef struct s_content_manager_t {
    controller_data_t controller_data;
    QueueHandle_t incoming_queue_commands;
    QueueHandle_t outgoing_queue_lcd;

    lv_obj_t *content_area;
    lv_obj_t *content_btn_label;
    btn_stage_t btn_stage;
    lv_obj_t *status_label;
    lv_obj_t *timer_label;

    content_t current_content;
    contents_t current_content_type;
} s_content_manager_t;

content_manager_t content_manager_init(controller_data_t controller_data, QueueHandle_t incoming_queue_commands, QueueHandle_t outgoing_queue_lcd, lv_obj_t *header, screen_manager_t screen_manager);
void content_manager_update(content_manager_t content_manager, controller_event_t incoming_event);
void content_manager_set_content(content_manager_t content_manager, contents_t content_type);

static lv_obj_t *btn_content_create(lv_obj_t *parent, content_manager_t content_manager);
static btn_stage_t btn_stage_create(lv_obj_t *parent, content_manager_t content_manager);

#endif
