#include "controls.h"

#include "components.h"
#include "content.h"
#include "lvgl.h"

content_controls_t content_controls_create(lv_obj_t *content_area, content_manager_t content_manager) {
    content_manager->current_content_type = CONTROLS;

    content_controls_t content_controls = (content_controls_t)malloc(sizeof(s_content_controls_t));

    content_controls->sensor_ar_label = sensor_ar_create(content_area, 26, 12, 28, 4, 58, 14);
    content_controls->sensor_grad_label = sensor_grad_create(content_area, 175, 12, 29, 10);
    content_controls->sensor_grao_label = sensor_grao_create(content_area, 322, 12, 27, 4, 47, 14);
    content_controls->arc_potencia_obj = arc_container_create(content_area, "POTENCIA", 26, 95, POTENCIA); content_controls->arc_cilindro_obj = arc_container_create(content_area, "CILINDRO", 175, 95, CILINDRO);
    content_controls->arc_turbina_obj = arc_container_create(content_area, "TURBINA", 322, 95, TURBINA);
    return content_controls;
}

void controls_update(content_controls_t content_controls) {
}

void controls_delete(content_controls_t content_controls) {
    // free(content_controls->controls_obj);
    // free(content_controls);
}

content_i controls_as_content = &(s_content_i){
    .update = (void *)controls_update,
    .delete = (void *)controls_delete};

lv_obj_t *sensor_create(lv_obj_t *parent, int x, int y) {
    lv_obj_t *container = container_create(parent);
    lv_obj_set_pos(container, x, y);
    lv_obj_set_size(container, 132, 80);
    lv_obj_set_style_radius(container, 10, 0);
    lv_obj_set_style_shadow_width(container, 5, 0);

    return container;
}

lv_obj_t *sensor_ar_create(lv_obj_t *parent, int x, int y, int label_temp_x, int label_temp_y, int label_ar_x, int label_ar_y) {
    lv_obj_t *container = sensor_create(parent, x, y);

    lv_obj_t *label_temp = lv_label_create(container);
    lv_obj_t *label_ar = lv_label_create(container);

    lv_obj_set_pos(label_temp, label_temp_x, label_temp_y);
    lv_obj_set_pos(label_ar, label_ar_x, label_ar_y);

    lv_label_set_text(label_temp, "TEMPERATURA");
    lv_obj_set_style_text_font(label_temp, &lv_font_montserrat_10, 0);

    lv_label_set_text(label_ar, "AR");
    lv_obj_set_style_text_font(label_ar, &lv_font_montserrat_12, 0);

    lv_obj_t *inner_container = container_create(container);
    lv_obj_set_pos(inner_container, 8, 31);
    lv_obj_set_size(inner_container, 116, 43);

    lv_obj_t *label_temp_value = lv_label_create(inner_container);
    lv_obj_set_style_text_font(label_temp_value, &lv_font_montserrat_20, 0);
    lv_label_set_text(label_temp_value, "0 C");
    lv_obj_center(label_temp_value);

    return label_temp_value;
}

lv_obj_t *sensor_grad_create(lv_obj_t *parent, int x, int y, int label_grad_x, int label_grad_y) {
    lv_obj_t *container = sensor_create(parent, x, y);

    lv_obj_t *label_grad = lv_label_create(container);

    lv_obj_set_pos(label_grad, label_grad_x, label_grad_y);

    lv_label_set_text(label_grad, "GRADIENTE");
    lv_obj_set_style_text_font(label_grad, &lv_font_montserrat_12, 0);

    lv_obj_t *inner_container = container_create(container);
    lv_obj_set_pos(inner_container, 8, 31);
    lv_obj_set_size(inner_container, 116, 43);

    lv_obj_t *label_temp_value = lv_label_create(inner_container);
    lv_obj_set_style_text_font(label_temp_value, &lv_font_montserrat_20, 0);
    lv_label_set_text(label_temp_value, "0 C");
    lv_obj_center(label_temp_value);

    return label_temp_value;
}

lv_obj_t *sensor_grao_create(lv_obj_t *parent, int x, int y, int label_temp_x, int label_temp_y, int label_grao_x, int label_grao_y) {
    lv_obj_t *container = sensor_create(parent, x, y);

    lv_obj_t *label_temp = lv_label_create(container);
    lv_obj_t *label_grao = lv_label_create(container);

    lv_obj_set_pos(label_temp, label_temp_x, label_temp_y);
    lv_obj_set_pos(label_grao, label_grao_x, label_grao_y);

    lv_label_set_text(label_temp, "TEMPERATURA");
    lv_obj_set_style_text_font(label_temp, &lv_font_montserrat_10, 0);

    lv_label_set_text(label_grao, "GRAO");
    lv_obj_set_style_text_font(label_grao, &lv_font_montserrat_12, 0);

    lv_obj_t *inner_container = container_create(container);
    lv_obj_set_pos(inner_container, 8, 31);
    lv_obj_set_size(inner_container, 116, 43);

    lv_obj_t *label_temp_value = lv_label_create(inner_container);
    lv_obj_set_style_text_font(label_temp_value, &lv_font_montserrat_20, 0);
    lv_label_set_text(label_temp_value, "0 C");
    lv_obj_center(label_temp_value);

    return label_temp_value;
}

void arc_event_value_changed_cb(lv_event_t *e) {
    lv_obj_t *arc = lv_event_get_target(e);
    arc_obj_t arc_obj = (arc_obj_t)lv_event_get_user_data(e);

/*
    use arc_obj->arc_type para mandar pra uma queue especifica
*/
    lv_label_set_text_fmt(arc_obj->arc_label, "%d", (int)lv_arc_get_value(arc));

    arc_obj->debounced = false;
}

void arc_event_released_cb(lv_event_t *e) {
    arc_obj_t arc_obj = (arc_obj_t)lv_event_get_user_data(e);

    arc_obj->debounced = true;
}


arc_obj_t arc_container_create(lv_obj_t *parent, char *label, int x, int y, arc_t arc_type) {
    arc_obj_t arc_obj = malloc(sizeof(s_arc_obj_t));

    lv_obj_t *container = container_create(parent);
    lv_obj_set_size(container, 132, 110);
    lv_obj_set_style_bg_color(container, lv_color_hex(0xF1FAEE), 0);
    lv_obj_set_pos(container, x, y);

    lv_obj_t *label_arc = lv_label_create(container);
    lv_label_set_text(label_arc, label);
    lv_obj_set_align(label_arc, LV_ALIGN_BOTTOM_MID);

    lv_obj_t *arc = lv_arc_create(container);
    lv_obj_set_size(arc, 85, 85);
    lv_arc_set_rotation(arc, 135);
    lv_arc_set_bg_angles(arc, 0, 270);
    lv_arc_set_value(arc, 0);
    lv_obj_center(arc);
    lv_obj_set_style_translate_y(arc, -5, 0);
    // lv_arc_set_change_rate(arc, 1);

    lv_obj_t *arc_label = lv_label_create(container);
    lv_label_set_text(arc_label, "0");
    lv_obj_set_style_text_font(arc_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_translate_y(arc_label, -10, 0);
    lv_obj_set_align(arc_label, LV_ALIGN_CENTER);

    lv_obj_t *arc_unit_label = lv_label_create(container);
    lv_label_set_text(arc_unit_label, "%");
    lv_obj_set_style_text_font(arc_unit_label, &lv_font_montserrat_14, 0);
    lv_obj_set_align(arc_unit_label, LV_ALIGN_CENTER);
    lv_obj_set_style_translate_y(arc_unit_label, 10, 0);

    arc_obj->arc_type = arc_type;
    arc_obj->arc_label = arc_label;
    arc_obj->debounced = true;

    lv_obj_add_event_cb(arc, arc_event_value_changed_cb, LV_EVENT_VALUE_CHANGED, arc_obj);
    lv_obj_add_event_cb(arc, arc_event_released_cb, LV_EVENT_RELEASED, arc_obj);

    return arc_obj;
}