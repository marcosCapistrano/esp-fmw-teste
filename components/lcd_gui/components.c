#include "components.h"

#include "lvgl.h"
#include "screen.h"

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

lv_obj_t *status_create(lv_obj_t *parent, const char *starting_status) {
    lv_obj_t *status_title = lv_label_create(parent);
    lv_label_set_text(status_title, "STATUS");
    lv_obj_set_pos(status_title, 221, 58);
    lv_obj_set_style_text_font(status_title, &lv_font_montserrat_10, 0);

    lv_obj_t *status = lv_label_create(parent);
    lv_label_set_text(status, starting_status);
    lv_obj_set_pos(status, 191, 68);
    lv_obj_set_style_text_font(status, &lv_font_montserrat_16, 0);

    return status;
}

lv_obj_t *chart_create(lv_obj_t *parent) {
    lv_obj_t *chart = lv_chart_create(parent);
    lv_obj_set_pos(chart, 50, 100);
    lv_obj_set_size(chart, 380, 188);
    lv_obj_set_style_bg_color(chart, lv_color_make(248, 248, 248), 0);
    lv_obj_set_style_border_width(chart, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 300);
    lv_chart_set_range(chart, LV_CHART_AXIS_SECONDARY_Y, -100, 100);
    lv_chart_set_point_count(chart, 12);

    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 12, 2, true, 30);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 6, 5, true, 50);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_SECONDARY_Y, 10, 5, 6, 2, true, 50);

    lv_chart_series_t *ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t *ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_SECONDARY_Y);

	return chart;
}