#ifndef CONTENT_CONTROLS_H
#define CONTENT_CONTROLS_H 

#include "lvgl.h"
#include "components.h"
#include "content.h"
#include "content_manager.h"

typedef enum {
	POTENCIA,
	CILINDRO,
	TURBINA
} arc_t;

typedef struct s_arc_obj_t *arc_obj_t;
typedef struct s_arc_obj_t {
		lv_obj_t *arc;
		arc_t arc_type;
    lv_obj_t *arc_label;
    _Bool debounced;
		int value;
} s_arc_obj_t;

typedef struct s_content_controls_t *content_controls_t;
typedef struct s_content_controls_t {
	content_manager_t content_manager;
	lv_obj_t *sensor_ar_label;
	lv_obj_t *sensor_grad_label;
	lv_obj_t *sensor_grao_label;

	arc_obj_t arc_potencia_obj;
	arc_obj_t arc_cilindro_obj;
	arc_obj_t arc_turbina_obj;

} s_content_controls_t;

content_i controls_as_content;

/*
	Forward refs
*/
typedef struct s_content_manager_t *content_manager_t;

content_controls_t content_controls_create(lv_obj_t *content_area, content_manager_t content_manager);

static lv_obj_t *sensor_ar_create(lv_obj_t *parent, int x, int y, int label_temp_x, int label_temp_y, int label_ar_x, int label_ar_y);

static lv_obj_t *sensor_grad_create(lv_obj_t *parent, int x, int y, int label_grad_x, int label_grad_y);

static lv_obj_t *sensor_grao_create(lv_obj_t *parent, int x, int y, int label_temp_x, int label_temp_y, int label_grao_x, int label_grao_y);


static arc_obj_t arc_container_create(lv_obj_t *parent, char *label, int x, int y, arc_t arc_type, QueueHandle_t incoming_queue_commands);

static void controls_init(content_controls_t content_controls);
#endif