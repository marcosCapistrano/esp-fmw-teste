#include "content_manager.h"

#include "chart.h"
#include "common_controller.h"
#include "components.h"
#include "content.h"
#include "controls.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lvgl.h"

#define INITIAL_CONTENT CHART

content_manager_t content_manager_init(controller_data_t controller_data, QueueHandle_t incoming_queue_commands, QueueHandle_t outgoing_queue_lcd, lv_obj_t *header, screen_manager_t screen_manager) {
    content_manager_t content_manager = (content_manager_t)malloc(sizeof(s_content_manager_t));
    content_manager->incoming_queue_commands = incoming_queue_commands;
    content_manager->outgoing_queue_lcd = outgoing_queue_lcd;
    content_manager->controller_data = controller_data;

    content_manager->content_btn_label = btn_content_create(header, content_manager);

    lv_obj_t *content_area = container_create(screen_manager->main_screen);
    lv_obj_set_pos(content_area, 0, 96);
    lv_obj_set_size(content_area, 480, 224);
    content_manager->content_area = content_area;

    content_manager->current_content = 0;
    content_manager_set_content(content_manager, INITIAL_CONTENT);

    content_manager->status_label = status_create(screen_manager->main_screen);
    content_manager->btn_stage = btn_stage_create(screen_manager->main_screen, content_manager);
    content_manager->timer_label = timer_label_create(screen_manager->main_screen);

    /*
    Inicia os valores quando o content for criado
    */
    content_manager->btn_stage->value = (controller_data->read_stage + 1) % 4;
    lv_label_set_text(content_manager->status_label, controller_stage_to_string(controller_data->read_stage));
    lv_label_set_text(content_manager->btn_stage->label, controller_stage_to_string_verb(content_manager->btn_stage->value));
    lv_label_set_text(content_manager->timer_label, "00:00");

    return content_manager;
}

void content_manager_update(content_manager_t content_manager, controller_event_t incoming_event) {
    ESP_LOGE("TAG", "CHEGANDOOO %d", (int)incoming_event);
    switch (incoming_event) {
        case STAGE_EVENT:
            content_manager->btn_stage->value = (content_manager->controller_data->read_stage + 1) % 4;
            lv_label_set_text(content_manager->btn_stage->label, controller_stage_to_string_verb(content_manager->btn_stage->value));

            lv_label_set_text(content_manager->status_label, controller_stage_to_string(content_manager->controller_data->read_stage));
            break;

        case TIMER_VALUE_EVENT: {
            uint64_t minutes = 0, seconds = 0;

            if (content_manager->controller_data->read_stage == START) {
                seconds = content_manager->controller_data->read_torra_time / 10E5;
                minutes = (seconds / 60) % 60;
            } else if (content_manager->controller_data->read_stage == COOLER) {
                seconds = content_manager->controller_data->read_resf_time / 10E5;
                minutes = (seconds / 60) % 60;
            }

            lv_label_set_text_fmt(content_manager->timer_label, "%02d:%02d", (int)minutes, (int)(seconds % 60));
            break;
        }
        case SENSOR_VALUE_EVENT:
        case ACTUATOR_VALUE_EVENT:
            content_update(content_manager->current_content, incoming_event);
            break;

        default:;
    }
}

void clear_content(content_manager_t content_manager) {
    if (content_manager->current_content != 0) {
        lv_obj_clean(content_manager->content_area);
        content_delete(content_manager->current_content);
    }
}

void content_manager_set_content(content_manager_t content_manager, contents_t content_type) {
    clear_content(content_manager);

    switch (content_type) {
        case CHART: {
            content_chart_t content_chart = content_chart_create(content_manager->content_area, content_manager);
            content_manager->current_content = content_create(content_chart, chart_as_content);
            break;
        }

        case CONTROLS: {
            content_controls_t content_controls = content_controls_create(content_manager->content_area, content_manager);
            content_manager->current_content = content_create(content_controls, controls_as_content);
        }

        default:

            break;
    }
}

void btn_content_event_handler(lv_event_t *e) {
    content_manager_t content_manager = (content_manager_t)lv_event_get_user_data(e);

    if (content_manager->current_content_type == CHART) {
        content_manager_set_content(content_manager, CONTROLS);
        lv_label_set_text(content_manager->content_btn_label, "GRAFICO");
    } else if (content_manager->current_content_type == CONTROLS) {
        content_manager_set_content(content_manager, CHART);
        lv_label_set_text(content_manager->content_btn_label, "CONTROLES");
    }
}

lv_obj_t *btn_content_create(lv_obj_t *parent, content_manager_t content_manager) {
    lv_obj_t *btn = btn_container_create(parent);
    lv_obj_set_pos(btn, 383, 10);
    lv_obj_set_size(btn, 70, 30);

    lv_obj_t *label_btn = lv_label_create(btn);
    if (content_manager->current_content_type == CONTROLS) {
        lv_label_set_text(label_btn, "GRAFICO");
    } else {
        lv_label_set_text(label_btn, "CONTROLES");
    }
    lv_obj_center(label_btn);
    lv_obj_set_style_text_font(label_btn, &lv_font_montserrat_10, 0);

    lv_obj_add_event_cb(btn, btn_content_event_handler, LV_EVENT_CLICKED, content_manager);

    return label_btn;
}

void btn_stage_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    content_manager_t content_manager = (content_manager_t)lv_event_get_user_data(e);
    btn_stage_t btn_stage = content_manager->btn_stage;

    incoming_data_t incoming_data = incoming_data_init();
    incoming_data->reader_type = LCD;

    incoming_data->state = ON;
    incoming_data->mode = MANUAL;
    incoming_data->stage = btn_stage->value;
    xQueueSend(content_manager->incoming_queue_commands, &incoming_data, portMAX_DELAY);
}

btn_stage_t btn_stage_create(lv_obj_t *parent, content_manager_t content_manager) {
    lv_obj_t *btn = btn_container_create(parent);
    lv_obj_set_size(btn, 88, 30);
    lv_obj_set_pos(btn, 26, 58);
    lv_obj_t *label = lv_label_create(btn);

    lv_label_set_text(label, " --- ");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_10, 0);
    lv_obj_center(label);

    btn_stage_t btn_stage = (btn_stage_t)malloc(sizeof(s_btn_stage_t));
    btn_stage->label = label;
    btn_stage->value = 0;
    lv_obj_add_event_cb(btn, btn_stage_event_handler, LV_EVENT_CLICKED, content_manager);

    return btn_stage;
}