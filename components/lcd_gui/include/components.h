#ifndef LCD_COMPONENTS_H
#define LCD_COMPONENTS_H

#include "lvgl.h"
#include "screen_manager.h"
#include "common_controller.h"

typedef struct s_btn_stage_t {
		lv_obj_t *btn;
		lv_obj_t *label;

		controller_stage_t read_stage;
		controller_stage_t write_stage;
} s_btn_stage_t;

typedef struct s_btn_stage_t *btn_stage_t;

/*Forward REFS*/
typedef struct s_screen_manager_t *screen_manager_t;

typedef struct s_status_obj_t {
	lv_label_t *label;
	char *value;
}	s_status_obj_t;

typedef struct s_chart_obj_t {
    lv_chart_t *chart;
} s_chart_obj_t;

typedef struct s_chart_obj_t *chart_obj_t;

typedef struct s_status_obj_t *status_obj_t;

lv_obj_t* btn_container_create(lv_obj_t *parent);
lv_obj_t* container_create(lv_obj_t *parent);
lv_obj_t *header_create(lv_obj_t *parent, const char *title, screen_manager_t screen_manager);
status_obj_t status_create(lv_obj_t *parent, const char* starting_status);
chart_obj_t chart_create(lv_obj_t *parent);
btn_stage_t btn_stage_create(lv_obj_t *parent);

#endif