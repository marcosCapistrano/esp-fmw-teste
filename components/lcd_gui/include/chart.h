#ifndef CONTENT_CHART_H
#define CONTENT_CHART_H 

#include "lvgl.h"
#include "components.h"
#include "content.h"
#include "content_manager.h"

typedef struct s_content_chart_t *content_chart_t;
typedef struct s_content_chart_t {
	chart_obj_t chart_obj;
} s_content_chart_t;

content_i chart_as_content;

/*
	Forward refs
*/
typedef struct s_content_manager_t *content_manager_t;

content_chart_t content_chart_create(lv_obj_t *content_area, content_manager_t content_manager);
#endif