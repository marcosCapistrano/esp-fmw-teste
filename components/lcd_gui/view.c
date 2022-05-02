#include "lvgl.h" 

static lv_obj_t * tv;
static lv_obj_t * t1;
static lv_obj_t * t2;
static lv_obj_t * t3;
static lv_obj_t * kb;

static lv_style_t style_box;

static void value_changed_event_cb(lv_event_t * e);

void lv_example_arc(lv_obj_t *view)
{
    lv_obj_t * label = lv_label_create(view);

    /*Create an Arc*/
    lv_obj_t * arc = lv_arc_create(view);
    lv_obj_set_size(arc, 144, 112);
    lv_arc_set_rotation(arc, 135);
    lv_arc_set_bg_angles(arc, 0, 270);
    lv_arc_set_value(arc, 10);
    lv_obj_center(arc);
    lv_obj_add_event_cb(arc, value_changed_event_cb, LV_EVENT_VALUE_CHANGED, label);

    /*Manually update the label for the first time*/
    lv_event_send(arc, LV_EVENT_VALUE_CHANGED, NULL);
}

static void value_changed_event_cb(lv_event_t * e)
{
    lv_obj_t * arc = lv_event_get_target(e);
    lv_obj_t * label = lv_event_get_user_data(e);

    lv_label_set_text_fmt(label, "%d%%", lv_arc_get_value(arc));

    /*Rotate the label to the current position of the arc*/
    lv_arc_rotate_obj_to_angle(arc, label, 25);
}

void view(void) {
	tv = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 45);
}