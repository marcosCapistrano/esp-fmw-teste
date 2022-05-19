#include "components.h"

#include "esp_log.h"
#include "lvgl.h"

static void btn_back_event_handler(lv_event_t *e);

lv_obj_t *container_create(lv_obj_t *parent) {
    lv_obj_t *container = lv_obj_create(parent);
    lv_obj_set_style_pad_all(container, 0, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_outline_width(container, 0, 0);

    return container;
}

lv_obj_t *btn_container_create(lv_obj_t *parent) {
    lv_obj_t *container = lv_btn_create(parent);
    lv_obj_set_style_pad_all(container, 0, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_outline_width(container, 0, 0);

    return container;
}

btn_stage_t btn_stage_create(lv_obj_t *parent) {
    lv_obj_t *btn = btn_container_create(parent);
    lv_obj_set_size(btn, 88, 30);
    lv_obj_set_pos(btn, 26, 58);
    lv_obj_t *label = lv_label_create(btn);

    lv_label_set_text(label, " --- ");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_10, 0);
    lv_obj_center(label);

    btn_stage_t btn_stage = (btn_stage_t)malloc(sizeof(s_btn_stage_t));
    btn_stage->btn = btn;
    btn_stage->label = label;
    btn_stage->read_value = STAGE_OFF;
    btn_stage->write_value = STAGE_NONE;

    return btn_stage;
}

label_timer_t label_timer_create(lv_obj_t *parent) {
    lv_obj_t *label = lv_label_create(parent);
    lv_obj_set_pos(label, 409, 61);

    lv_label_set_text(label, "--:--");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);

    label_timer_t label_timer = (label_timer_t)malloc(sizeof(s_label_timer_t));
    label_timer->label = label;
    label_timer->read_value = 0;

    return label_timer;
}

lv_obj_t *header_create(lv_obj_t *parent, const char *title, screen_manager_t screen_manager) {
    lv_obj_t *header = container_create(parent);
    lv_obj_set_size(header, lv_obj_get_width(parent), 50);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x457B9D), 0);

    lv_obj_t *title_label = lv_label_create(header);
    lv_label_set_text(title_label, title);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0xFFFFFF), 0);

    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_20, 0);
    lv_obj_center(title_label);

    lv_obj_t *btn_back = btn_container_create(header);
    lv_obj_set_size(btn_back, 70, 30);
    lv_obj_set_pos(btn_back, 26, 10);

    lv_obj_t *label_btn_back = lv_label_create(btn_back);
    lv_obj_set_pos(label_btn_back, 15, 9);
    lv_obj_set_style_text_font(label_btn_back, &lv_font_montserrat_10, 0);
    lv_label_set_text(label_btn_back, "VOLTAR");
    lv_obj_add_event_cb(btn_back, btn_back_event_handler, LV_EVENT_ALL, screen_manager);

    return header;
}

void btn_back_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    screen_manager_t screen_manager = (screen_manager_t)lv_event_get_user_data(e);

    if (code == LV_EVENT_CLICKED) {
        screen_manager_go_back(screen_manager);
    }
}

status_obj_t status_create(lv_obj_t *content) {
    status_obj_t status_obj = malloc(sizeof(s_status_obj_t));

    lv_obj_t *status_title = lv_label_create(content);
    lv_label_set_text(status_title, "STATUS");
    lv_obj_set_pos(status_title, 221, 58);
    lv_obj_set_style_text_font(status_title, &lv_font_montserrat_10, 0);

    lv_obj_t *status = lv_label_create(content);
    lv_obj_set_pos(status, 191, 68);
    lv_obj_set_style_text_font(status, &lv_font_montserrat_16, 0);

    status_obj->label = status;
    status_obj->write_value = STAGE_NONE;

    return status_obj;
}

chart_obj_t chart_create(lv_obj_t *parent) {
    chart_obj_t chart_obj = malloc(sizeof(s_chart_obj_t));

    lv_obj_t *chart = lv_chart_create(parent);
    lv_obj_set_pos(chart, 50, 0);
    lv_obj_set_size(chart, 380, 188);
    lv_obj_set_style_bg_color(chart, lv_color_make(248, 248, 248), 0);
    lv_obj_center(chart);
    lv_obj_set_style_border_width(chart, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);

    // lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_X, 0, CONTROLLER_MAX_TIME_MINS * 2 + 2);
    // lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 10);
    // lv_chart_set_range(chart, LV_CHART_AXIS_SECONDARY_Y, -10, 10);

    lv_chart_series_t *temp_ar_series = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);      // temp ar
    lv_chart_series_t *temp_grao_series = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);     // temp grao
    lv_chart_series_t *grad_series = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_PINK), LV_CHART_AXIS_PRIMARY_Y);          // grad
    lv_chart_series_t *delta_grao_series = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_PINK), LV_CHART_AXIS_SECONDARY_Y);  // grad

    // lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 5, 2, 2, 5, true, 20);
    // lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 5, 2, 2, 6, true, 20);
    // lv_chart_set_axis_tick(chart, LV_CHART_AXIS_SECONDARY_Y, 5, 2, 0, 0, true, 20);

    chart_obj->chart = chart;
    chart_obj->temp_ar_series = temp_ar_series;
    chart_obj->temp_grao_series = temp_grao_series;
    chart_obj->grad_series = grad_series;
    chart_obj->delta_grao_series = delta_grao_series;

    return chart_obj;
}

void chart_draw_pre_heating(chart_obj_t chart_obj, int value) {
    lv_obj_t *chart = chart_obj->chart;

    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 5, 2, 2, 6, true, 40);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, value);

    chart_obj->temp_ar_series->y_points[0] = value;
    lv_chart_refresh(chart);
    ESP_LOGE("OI", "CHAMANDO?:");
}