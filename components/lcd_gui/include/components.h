#ifndef LCD_COMPONENTS_H
#define LCD_COMPONENTS_H

#define CONTROLLER_MAX_TIME_MINS 25

#include "common_controller.h"
#include "lvgl.h"
#include "screen_manager.h"

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
    lv_obj_t *chart;

    lv_chart_series_t *temp_grao_series;
    lv_chart_series_t *temp_ar_series;
    lv_chart_series_t *grad_series;
    lv_chart_series_t *delta_grao_series;

    int point_count;
    int lower_limit;
    int upper_limit;

} s_chart_obj_t;

typedef struct s_chart_obj_t *chart_obj_t;

typedef struct s_status_obj_t *status_obj_t;

typedef struct s_content_manager_t *content_manager_t;

lv_obj_t *btn_container_create(lv_obj_t *parent);
lv_obj_t *container_create(lv_obj_t *parent);
lv_obj_t *header_create(lv_obj_t *parent, const char *title, screen_manager_t screen_manager);
lv_obj_t *status_create(lv_obj_t *parent);
chart_obj_t chart_create(lv_obj_t *parent);
lv_obj_t *timer_label_create(lv_obj_t *parent);

void chart_draw_pre_heating(chart_obj_t chart_obj, int value);
void chart_draw_start(chart_obj_t chart_obj, controller_data_t controller_data);

static void header_btn_back_event_handler(lv_event_t *e);
#endif