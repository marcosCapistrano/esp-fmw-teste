#ifndef LCD_COMPONENTS_H
#define LCD_COMPONENTS_H

#include "lvgl.h"
#include "screen.h"

typedef struct s_btn_stage_t {
		lv_obj_t *btn;
		lv_obj_t *label;
} s_btn_stage_t;

typedef struct s_btn_stage_t *btn_stage_t;

lv_obj_t* btn_container_create(lv_obj_t *parent);
lv_obj_t* container_create(lv_obj_t *parent);
lv_obj_t *header_create(lv_obj_t *parent, const char *title, screen_manager_t screen_manager);
lv_obj_t* status_create(lv_obj_t *parent, const char* starting_status);
lv_obj_t* chart_create(lv_obj_t *parent);
btn_stage_t btn_stage_create(lv_obj_t *parent);

#endif