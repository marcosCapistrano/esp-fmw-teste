#ifndef LCD_COMPONENTS_H
#define LCD_COMPONENTS_H

#include "common_controller.h"
#include "lvgl.h"
#include "screen_manager.h"

typedef struct s_btn_stage_t {
    lv_obj_t *btn;
    lv_obj_t *label;

    controller_stage_t read_value;
    controller_stage_t write_value;
} s_btn_stage_t;

typedef struct s_btn_stage_t *btn_stage_t;

typedef struct s_label_timer_t {
    lv_obj_t *label;

    time_t read_value;
} s_label_timer_t;

typedef struct s_label_timer_t *label_timer_t;

/*Forward REFS*/
typedef struct s_screen_manager_t *screen_manager_t;

typedef struct s_status_obj_t {
    lv_obj_t *label;
    controller_stage_t read_value;
    controller_stage_t write_value;
} s_status_obj_t;

typedef struct s_chart_obj_t {
    lv_chart_t *chart;
} s_chart_obj_t;

typedef struct s_chart_obj_t *chart_obj_t;

typedef struct s_status_obj_t *status_obj_t;

lv_obj_t *btn_container_create(lv_obj_t *parent);
lv_obj_t *container_create(lv_obj_t *parent);
lv_obj_t *header_create(lv_obj_t *parent, const char *title, screen_manager_t screen_manager);
status_obj_t status_create(lv_obj_t *parent);
chart_obj_t chart_create(lv_obj_t *parent);
btn_stage_t btn_stage_create(lv_obj_t *parent);
label_timer_t label_timer_create(lv_obj_t *parent);
#endif