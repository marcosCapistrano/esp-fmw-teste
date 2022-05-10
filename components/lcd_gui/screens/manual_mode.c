#include "manual_mode.h"

#include <lvgl_esp32_drivers/lvgl_helpers.h>

#include "components.h"
#include "lvgl.h"
#include "screen.h"

lv_obj_t *sensor_create(lv_obj_t *parent, int x, int y);
lv_obj_t *sensor_ar_create(lv_obj_t *parent, int x, int y, int label_temp_x, int label_temp_y, int label_ar_x, int label_ar_y);
lv_obj_t *sensor_grao_create(lv_obj_t *parent, int x, int y, int label_temp_x, int label_temp_y, int label_grao_x, int label_grao_y);
lv_obj_t *sensor_grad_create(lv_obj_t *parent, int x, int y, int label_grao_x, int label_grao_y);
lv_obj_t *arc_container_create(lv_obj_t *parent, char *label, int x, int y);
btn_content_t btn_content_create(lv_obj_t *parent);

void controls_create(lv_obj_t *parent);
void btn_content_event_handler(lv_event_t *e);
void btn_stage_event_handler(lv_event_t *e);

void screen_manual_mode_init(lv_obj_t *main_screen, screen_manager_t screen_manager) {
    lv_obj_t *header = header_create(main_screen, "MODO MANUAL", screen_manager);
    lv_obj_t *status = status_create(main_screen, "DESLIGADO");
    btn_stage_t btn_stage = btn_stage_create(main_screen);

    lv_obj_t *content = container_create(main_screen);
    lv_obj_set_pos(content, 0, 96);
    lv_obj_set_size(content, 480, 224);

    btn_content_t btn_content = btn_content_create(header);

    content_manager_t content_manager = malloc(sizeof(s_content_manager_t));
    content_manager->current_content = GRAPH;
    content_manager->content = content;
    content_manager->btn_content = btn_content;

    lv_obj_add_event_cb(btn_content->btn, btn_content_event_handler, LV_EVENT_ALL, content_manager);
    lv_obj_add_event_cb(btn_stage->btn, btn_stage_event_handler, LV_EVENT_ALL, screen_manager);

    manual_mode_obj_t manual_mode_obj = malloc(sizeof(s_manual_mode_obj_t));
    manual_mode_obj->content_manager = content_manager;
    screen_manager->manual_mode_obj = manual_mode_obj;

    chart_create(content);
}

void clear_content(lv_obj_t *content) {
    lv_obj_clean(content);
}

void manual_mode_set_content(content_manager_t content_manager, content_t type) {
    clear_content(content_manager->content);
    content_manager->current_content = type;

    switch (type) {
        case GRAPH:
            chart_create(content_manager->content);
            break;

        case CONTROLS:
            controls_create(content_manager->content);
        default:

            break;
    }
}

void controls_create(lv_obj_t *content) {
    lv_obj_t *sensor_ar = sensor_ar_create(content, 26, 12, 28, 4, 58, 14);
    lv_obj_t *senor_grad = sensor_grad_create(content, 175, 12, 29, 10);
    lv_obj_t *sensor_grao = sensor_grao_create(content, 322, 12, 27, 4, 47, 14);
    lv_obj_t *arc_potencia = arc_container_create(content, "POTENCIA", 26, 104);
    lv_obj_t *arc_cilindro = arc_container_create(content, "CILINDRO", 175, 104);
    lv_obj_t *arc_turbina = arc_container_create(content, "TURBINA", 322, 104);
}

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

    return container;
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

    return container;
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

    return container;
}

lv_obj_t *arc_container_create(lv_obj_t *parent, char *label, int x, int y) {
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
    lv_arc_set_value(arc, 10);
    lv_obj_center(arc);
    lv_obj_set_style_translate_y(arc, -5, 0);

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

    // arc_event_user_data_t *user_data = malloc(sizeof(arc_event_user_data_t));
    // user_data->input_type = input_type;
    // user_data->label = arc_label;

    // lv_obj_add_event_cb(arc, arc_event_cb, LV_EVENT_VALUE_CHANGED, user_data);
    lv_event_send(arc, LV_EVENT_VALUE_CHANGED, NULL);

    return container;
}

void btn_stage_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    screen_manager_t screen_manager = (screen_manager_t)lv_event_get_user_data(e);

    manual_mode_obj_t manual_mode_obj = screen_manager->manual_mode_obj;
    content_manager_t content_manager = manual_mode_obj->content_manager;
    
    QueueHandle_t input_control_queue = screen_manager->input_control_queue;

    input_event_t input_event = malloc(sizeof(s_input_event_t));
    input_event->event = STAGE;
    input_event->stage = PRE_HEATING; 
    input_event->input = INPUT_NONE;
    input_event->value = 0;

    if (code == LV_EVENT_CLICKED) {
       xQueueSend(input_control_queue, &input_event , 0);
    }
}

void btn_content_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    content_manager_t content_manager = (content_manager_t)lv_event_get_user_data(e);

    if (code == LV_EVENT_CLICKED) {
        if (content_manager->current_content == GRAPH) {
            manual_mode_set_content(content_manager, CONTROLS);
            lv_label_set_text(content_manager->btn_content->label, "GRAFICO");
        } else {
            manual_mode_set_content(content_manager, GRAPH);
            lv_label_set_text(content_manager->btn_content->label, "CONTROLES");
        }
    }
}

btn_content_t btn_content_create(lv_obj_t *parent) {
    lv_obj_t *btn = btn_container_create(parent);
    lv_obj_set_pos(btn, 383, 10);
    lv_obj_set_size(btn, 70, 30);

    lv_obj_t *label_btn = lv_label_create(btn);
    lv_label_set_text(label_btn, "CONTROLES");
    lv_obj_center(label_btn);
    lv_obj_set_style_text_font(label_btn, &lv_font_montserrat_10, 0);

    btn_content_t btn_content = malloc(sizeof(s_btn_content_t));
    btn_content->btn = btn;
    btn_content->label = label_btn;

    return btn_content;
}