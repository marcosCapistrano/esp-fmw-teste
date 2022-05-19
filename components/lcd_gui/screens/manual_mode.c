#include "manual_mode.h"

#include <lvgl_esp32_drivers/lvgl_helpers.h>

#include "components.h"
#include "esp_log.h"
#include "lvgl.h"
#include "screen_manager.h"

static const char *TAG = "MANUAL_MODE";

lv_obj_t *sensor_create(lv_obj_t *parent, int x, int y);
sensor_obj_t sensor_ar_create(lv_obj_t *parent, int x, int y, int label_temp_x, int label_temp_y, int label_ar_x, int label_ar_y);
sensor_obj_t sensor_grao_create(lv_obj_t *parent, int x, int y, int label_temp_x, int label_temp_y, int label_grao_x, int label_grao_y);
sensor_obj_t sensor_grad_create(lv_obj_t *parent, int x, int y, int label_grao_x, int label_grao_y);
arc_obj_t arc_container_create(lv_obj_t *parent, char *label, int x, int y);
btn_content_t btn_content_create(lv_obj_t *parent);

content_manager_t content_manager_create(lv_obj_t *parent, lv_obj_t *header);
void update_stage(screen_manager_t screen_manager, content_manager_t content_manager);
void update_controls(screen_manager_t screen_manager, content_manager_t content_manager);
void update_chart(screen_manager_t screen_manager, content_manager_t content_manager);
void controls_create(content_manager_t content_manager, lv_obj_t *parent);
void btn_content_event_handler(lv_event_t *e);
void btn_stage_event_handler(lv_event_t *e);
void manual_mode_set_content(content_manager_t content_manager, content_t type);

manual_mode_obj_t screen_manual_mode_init(lv_obj_t *main_screen, screen_manager_t screen_manager) {
    lv_obj_t *header = header_create(main_screen, "MODO MANUAL", screen_manager);

    manual_mode_obj_t manual_mode_obj = malloc(sizeof(s_manual_mode_obj_t));
    manual_mode_obj->status_obj = status_create(main_screen);
    manual_mode_obj->content_manager = content_manager_create(main_screen, header);

    screen_manager->current_screen = MANUAL_MODE;

    return manual_mode_obj;
}

void screen_manual_mode_update(screen_manager_t screen_manager) {
    manual_mode_obj_t manual_mode_obj = screen_manager->manual_mode_obj;
    content_manager_t content_manager = manual_mode_obj->content_manager;

    switch (content_manager->current_content) {
        case GRAPH:
            update_chart(screen_manager, content_manager);
            break;

        case CONTROLS:
            update_controls(screen_manager, content_manager);
            break;

        default:

            break;
    }

    update_stage(screen_manager, content_manager);
}

void update_stage(screen_manager_t screen_manager, content_manager_t content_manager) {
    controller_data_t controller_data = screen_manager->controller_data;
    manual_mode_obj_t manual_mode_obj = screen_manager->manual_mode_obj;

    status_obj_t status_obj = manual_mode_obj->status_obj;
    btn_stage_t btn_stage = content_manager->btn_stage;
    btn_content_t btn_content = content_manager->btn_content;
    label_timer_t label_timer = content_manager->label_timer;

    controller_stage_t read_stage = controller_data->read_stage;

    uint64_t minutes, seconds;
    seconds = controller_data->elapsed_time / 10E5;
    minutes = (seconds / 60) % 60;

    lv_label_set_text_fmt(label_timer->label, "%02d:%02d", (int)minutes, (int)(seconds % 60));

    if (status_obj->read_value != read_stage) {
        status_obj->read_value = read_stage;
        lv_label_set_text(status_obj->label, controller_stage_to_string(read_stage));
    }

    if (btn_content->read_value != btn_content->write_value) {
        ESP_LOGE(TAG, "Setting content!");
        manual_mode_set_content(content_manager, btn_content->write_value);

        if (btn_content->write_value == CONTROLS) {
            lv_label_set_text(content_manager->btn_content->label, "GRAFICO");
        } else if (btn_content->write_value == GRAPH) {
            lv_label_set_text(content_manager->btn_content->label, "CONTROLES");
        }

        btn_content->read_value = btn_content->write_value;
    }

    if (btn_stage->read_value != (read_stage + 1) % 5) {
        content_manager->btn_stage->read_value = (read_stage + 1) % 5;
        lv_label_set_text(content_manager->btn_stage->label, controller_stage_to_string_verb(btn_stage->read_value));
    }

    if (btn_stage->write_value != STAGE_NONE) {
        incoming_data_t incoming_data = incoming_data_init();
        incoming_data->reader_type = LCD;

        incoming_data->state = ON;
        incoming_data->mode = MANUAL;
        incoming_data->stage = btn_stage->write_value;

        xQueueSend(screen_manager->incoming_queue_commands, &incoming_data, portMAX_DELAY);

        content_manager->btn_stage->write_value = STAGE_NONE;
    }
}

void update_chart(screen_manager_t screen_manager, content_manager_t content_manager) {
    controller_data_t controller_data = screen_manager->controller_data;
    chart_obj_t chart_obj = content_manager->chart_obj;

    int temp_ar = controller_data->read_temp_ar;
    int temp_grao = controller_data->read_temp_grao;
    int grad = controller_data->read_grad;

    if (controller_data->read_state == ON) {
        switch (controller_data->read_stage) {
            case PRE_HEATING:
                chart_draw_pre_heating(chart_obj, controller_data->read_temp_ar);
                break;

            case START:
                chart_draw_start(chart_obj, controller_data);
                break;

            default:;
        }
    }
}

void update_controls(screen_manager_t screen_manager, content_manager_t content_manager) {
    controller_data_t controller_data = screen_manager->controller_data;

    int potencia = controller_data->read_potencia;
    int cilindro = controller_data->read_cilindro;
    int turbina = controller_data->read_turbina;

    int temp_ar = controller_data->read_temp_ar;
    int temp_grao = controller_data->read_temp_grao;
    int grad = controller_data->read_grad;

    sensor_obj_t sensor_ar_obj = content_manager->sensor_ar_obj;
    sensor_obj_t sensor_grao_obj = content_manager->sensor_grao_obj;
    sensor_obj_t sensor_grad_obj = content_manager->sensor_grad_obj;

    arc_obj_t arc_potencia_obj = content_manager->arc_potencia_obj;
    arc_obj_t arc_cilindro_obj = content_manager->arc_cilindro_obj;
    arc_obj_t arc_turbina_obj = content_manager->arc_turbina_obj;

    if (sensor_ar_obj->read_value != temp_ar) {
        sensor_ar_obj->read_value = temp_ar;
        lv_label_set_text_fmt(sensor_ar_obj->label, "%d", temp_ar);
    }

    if (sensor_grao_obj->read_value != temp_grao) {
        sensor_grao_obj->read_value = temp_grao;
        lv_label_set_text_fmt(sensor_grao_obj->label, "%d", temp_grao);
    }

    if (sensor_grad_obj->read_value != grad) {
        sensor_grad_obj->read_value = grad;
        lv_label_set_text_fmt(sensor_grad_obj->label, "%d", grad);
    }

    if (arc_potencia_obj->read_value != potencia) {
        arc_potencia_obj->read_value = potencia;
        lv_label_set_text_fmt(arc_potencia_obj->arc_label, "%d", potencia);
    }

    if (arc_potencia_obj->write_value != -1 && arc_potencia_obj->debounced) {
        incoming_data_t incoming_data = incoming_data_init();
        incoming_data->reader_type = LCD;
        incoming_data->write_potencia = arc_potencia_obj->write_value;

        xQueueSend(screen_manager->incoming_queue_commands, &incoming_data, portMAX_DELAY);

        arc_potencia_obj->write_value = -1;
        ESP_LOGE("3", "write_value %d", arc_potencia_obj->write_value);
    }

    if (arc_cilindro_obj->read_value != cilindro) {
        arc_cilindro_obj->read_value = cilindro;
        lv_label_set_text_fmt(arc_cilindro_obj->arc_label, "%d", cilindro);
    }

    if (arc_cilindro_obj->write_value != -1 && arc_cilindro_obj->debounced) {
        incoming_data_t incoming_data = incoming_data_init();
        incoming_data->reader_type = LCD;
        incoming_data->write_cilindro = arc_cilindro_obj->write_value;

        xQueueSend(screen_manager->incoming_queue_commands, &incoming_data, portMAX_DELAY);

        arc_cilindro_obj->write_value = -1;
    }

    if (arc_turbina_obj->read_value != turbina) {
        arc_turbina_obj->read_value = turbina;
        lv_label_set_text_fmt(arc_turbina_obj->arc_label, "%d", turbina);
    }

    if (arc_turbina_obj->write_value != -1 && arc_turbina_obj->debounced) {
        incoming_data_t incoming_data = incoming_data_init();
        incoming_data->reader_type = LCD;
        incoming_data->write_turbina = arc_turbina_obj->write_value;

        xQueueSend(screen_manager->incoming_queue_commands, &incoming_data, portMAX_DELAY);

        arc_turbina_obj->write_value = -1;
    }
}

content_manager_t content_manager_create(lv_obj_t *main_screen, lv_obj_t *header) {
    btn_stage_t btn_stage = btn_stage_create(main_screen);
    label_timer_t label_timer = label_timer_create(main_screen);

    lv_obj_t *content = container_create(main_screen);
    lv_obj_set_pos(content, 0, 96);
    lv_obj_set_size(content, 480, 224);

    btn_content_t btn_content = btn_content_create(header);

    content_manager_t content_manager = malloc(sizeof(s_content_manager_t));
    content_manager->content = content;
    content_manager->btn_content = btn_content;
    content_manager->btn_stage = btn_stage;
    content_manager->label_timer = label_timer;

    chart_create(content_manager, content);

    lv_obj_add_event_cb(btn_content->btn, btn_content_event_handler, LV_EVENT_CLICKED, btn_content);
    lv_obj_add_event_cb(btn_stage->btn, btn_stage_event_handler, LV_EVENT_CLICKED, btn_stage);

    content_manager->current_content = GRAPH;
    return content_manager;
}

void clear_content(lv_obj_t *content) {
    lv_obj_clean(content);
}

void manual_mode_set_content(content_manager_t content_manager, content_t type) {
    content_manager->current_content = type;

    clear_content(content_manager->content);
    ESP_LOGE(TAG, "content cleared");

    switch (type) {
        case GRAPH:
            chart_create(content_manager, content_manager->content);
            ESP_LOGE(TAG, "chart created");
            break;

        case CONTROLS:
            controls_create(content_manager, content_manager->content);
            ESP_LOGE(TAG, "controls created");
            break;

        default:

            break;
    }
}

void controls_create(content_manager_t content_manager, lv_obj_t *content) {
    content_manager->sensor_ar_obj = sensor_ar_create(content, 26, 12, 28, 4, 58, 14);
    content_manager->sensor_grad_obj = sensor_grad_create(content, 175, 12, 29, 10);
    content_manager->sensor_grao_obj = sensor_grao_create(content, 322, 12, 27, 4, 47, 14);
    content_manager->arc_potencia_obj = arc_container_create(content, "POTENCIA", 26, 95);
    content_manager->arc_cilindro_obj = arc_container_create(content, "CILINDRO", 175, 95);
    content_manager->arc_turbina_obj = arc_container_create(content, "TURBINA", 322, 95);
}

lv_obj_t *sensor_create(lv_obj_t *parent, int x, int y) {
    lv_obj_t *container = container_create(parent);
    lv_obj_set_pos(container, x, y);
    lv_obj_set_size(container, 132, 80);
    lv_obj_set_style_radius(container, 10, 0);
    lv_obj_set_style_shadow_width(container, 5, 0);

    return container;
}

sensor_obj_t sensor_ar_create(lv_obj_t *parent, int x, int y, int label_temp_x, int label_temp_y, int label_ar_x, int label_ar_y) {
    sensor_obj_t sensor_ar_obj = malloc(sizeof(s_sensor_obj_t));

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

    sensor_ar_obj->label = label_temp_value;
    sensor_ar_obj->read_value = 0;

    return sensor_ar_obj;
}

sensor_obj_t sensor_grad_create(lv_obj_t *parent, int x, int y, int label_grad_x, int label_grad_y) {
    sensor_obj_t sensor_grad_obj = malloc(sizeof(s_sensor_obj_t));
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

    sensor_grad_obj->label = label_temp_value;
    sensor_grad_obj->read_value = 0;

    return sensor_grad_obj;
}

sensor_obj_t sensor_grao_create(lv_obj_t *parent, int x, int y, int label_temp_x, int label_temp_y, int label_grao_x, int label_grao_y) {
    sensor_obj_t sensor_grao_obj = malloc(sizeof(s_sensor_obj_t));

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

    sensor_grao_obj->label = label_temp_value;
    sensor_grao_obj->read_value = 0;

    return sensor_grao_obj;
}

void arc_event_value_changed_cb(lv_event_t *e) {
    lv_obj_t *arc = lv_event_get_target(e);
    arc_obj_t arc_obj = (arc_obj_t)lv_event_get_user_data(e);

    lv_label_set_text_fmt(arc_obj->arc_label, "%d", (int)lv_arc_get_value(arc));

    ESP_LOGE("1", "write_value %d", (int)lv_arc_get_value(arc));
    arc_obj->write_value = lv_arc_get_value(arc);
    arc_obj->debounced = false;
}

void arc_event_released_cb(lv_event_t *e) {
    arc_obj_t arc_obj = (arc_obj_t)lv_event_get_user_data(e);

    arc_obj->debounced = true;
}

arc_obj_t arc_container_create(lv_obj_t *parent, char *label, int x, int y) {
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

    arc_obj->arc_label = arc_label;
    arc_obj->read_value = 0;
    arc_obj->write_value = -1;
    arc_obj->debounced = true;

    lv_obj_add_event_cb(arc, arc_event_value_changed_cb, LV_EVENT_VALUE_CHANGED, arc_obj);
    lv_obj_add_event_cb(arc, arc_event_released_cb, LV_EVENT_RELEASED, arc_obj);

    return arc_obj;
}

void btn_content_event_handler(lv_event_t *e) {
    btn_content_t btn_content = (btn_content_t)lv_event_get_user_data(e);

    if (btn_content->read_value == GRAPH) {
        btn_content->write_value = CONTROLS;
    } else if (btn_content->read_value == CONTROLS) {
        btn_content->write_value = GRAPH;
    }
}

void btn_stage_event_handler(lv_event_t *e) {
    btn_stage_t btn_stage = (btn_stage_t)lv_event_get_user_data(e);

    btn_stage->write_value = btn_stage->read_value;
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
    btn_content->write_value = GRAPH;
    btn_content->read_value = GRAPH;

    return btn_content;
}