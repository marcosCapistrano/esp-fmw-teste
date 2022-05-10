#ifndef SCREEN_MANUAL_MODE_H
#define SCREEN_MANUAL_MODE_H

#include "lvgl.h"
#include "screen.h"
#include "components.h"

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

typedef struct s_content_manager_t {
	content_t current_content;
	lv_obj_t *content;
	btn_content_t btn_content;
	btn_stage_t btn_stage;
} s_content_manager_t;

typedef struct s_content_manager_t *content_manager_t;

typedef struct s_manual_mode_obj_t {
	content_manager_t content_manager;
} s_manual_mode_obj_t;

typedef struct s_manual_mode_obj_t *manual_mode_obj_t;

void screen_manual_mode_init(lv_obj_t* main_screen, screen_manager_t screen_manager);

#endif