#ifndef SCREEN_MANUAL_MODE_H
#define SCREEN_MANUAL_MODE_H

#include "components.h"
#include "lvgl.h"
#include "screen_manager.h"

typedef enum {
    GRAPH,
    CONTROLS
} content_t;

typedef struct s_btn_content_t {
    lv_obj_t *btn;
    lv_obj_t *label;
} s_btn_content_t;

typedef struct s_btn_content_t *btn_content_t;

typedef struct s_btn_stage_t *btn_stage_t;

typedef struct s_sensor_obj_t {
    lv_obj_t *label;
    int read_value;
    int write_value;
} s_sensor_obj_t;

typedef struct s_sensor_obj_t *sensor_obj_t;

typedef struct s_arc_obj_t {
    lv_obj_t *arc_label;
    int read_value;
    int write_value;
} s_arc_obj_t;

typedef struct s_arc_obj_t *arc_obj_t;

typedef struct s_chart_obj_t *chart_obj_t;

typedef struct s_content_manager_t {
    content_t current_content;
    lv_obj_t *content;
    btn_content_t btn_content;
    btn_stage_t btn_stage;

    sensor_obj_t sensor_ar_obj;
    sensor_obj_t sensor_grao_obj;
    sensor_obj_t sensor_grad_obj;

    arc_obj_t arc_potencia_obj;
    arc_obj_t arc_cilindro_obj;
    arc_obj_t arc_turbina_obj;

    chart_obj_t chart_obj;
} s_content_manager_t;

typedef struct s_content_manager_t *content_manager_t;

typedef struct s_status_obj_t *status_obj_t;
typedef struct s_screen_manager_t *screen_manager_t;

typedef struct s_manual_mode_obj_t {
    content_manager_t content_manager;
    status_obj_t status_obj;
} s_manual_mode_obj_t;

typedef struct s_manual_mode_obj_t *manual_mode_obj_t;

manual_mode_obj_t screen_manual_mode_init(lv_obj_t *main_screen, screen_manager_t screen_manager);
void screen_manual_mode_update(screen_manager_t screen_manager, manual_mode_obj_t manual_mode_obj);

#endif