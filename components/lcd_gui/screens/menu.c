#include "menu.h"

#include <lvgl_esp32_drivers/lvgl_helpers.h>

#include "components.h"
#include "lvgl.h"
#include "screen_manager.h"

static btn_menu_small_t btn_menu_small_create(lv_obj_t *parent, int x, int y, const char *subtitle, int subtitle_x, int subtitle_y);
static btn_menu_big_t btn_menu_big_create(lv_obj_t *parent);
static void btn_manual_event_handler(lv_event_t *e);

menu_obj_t screen_menu_init(lv_obj_t *main_screen, screen_manager_t screen_manager) {
    btn_menu_small_t btn_manual = btn_menu_small_create(main_screen, 40, 45, "MANUAL", 44, 45);
    lv_obj_add_event_cb(btn_manual->btn, btn_manual_event_handler, LV_EVENT_ALL, screen_manager);

    btn_menu_small_t btn_auto = btn_menu_small_create(main_screen, 260, 45, "AUTOMATICO", 21, 45);
    btn_menu_big_t btn_history = btn_menu_big_create(main_screen);

    menu_obj_t menu_obj = malloc(sizeof(s_menu_obj_t));
    menu_obj->btn_menu_small[0] = btn_manual;
    menu_obj->btn_menu_small[1] = btn_auto;
    menu_obj->btn_menu_big = btn_history;

    screen_manager->current_screen = MENU;

    return menu_obj;
}

btn_menu_small_t btn_menu_small_create(lv_obj_t *parent, int x, int y, const char *subtitle, int subtitle_x, int subtitle_y) {
    lv_obj_t *btn = btn_container_create(parent);
    lv_obj_set_size(btn, 180, 100);
    lv_obj_set_pos(btn, x, y);

    lv_obj_t *title_label = lv_label_create(btn);
    lv_label_set_text(title_label, "MODO");
    lv_obj_set_pos(title_label, 69, 31);

    lv_obj_t *subtitle_label = lv_label_create(btn);
    lv_label_set_text(subtitle_label, subtitle);
    lv_obj_set_pos(subtitle_label, subtitle_x, subtitle_y);

    lv_style_t *title_style = (lv_style_t *)malloc(sizeof(lv_style_t));
    lv_style_init(title_style);
    lv_style_set_text_font(title_style, &lv_font_montserrat_12);
    lv_obj_add_style(title_label, title_style, 0);

    lv_style_t *subtitle_style = (lv_style_t *)malloc(sizeof(lv_style_t));
    lv_style_init(subtitle_style);
    lv_style_set_text_font(subtitle_style, &lv_font_montserrat_20);
    lv_obj_add_style(subtitle_label, subtitle_style, 0);

    btn_menu_small_t btn_menu_small = (btn_menu_small_t)malloc(sizeof(s_btn_menu_small_t));
    btn_menu_small->btn = btn;
    btn_menu_small->title_style = title_style;
    btn_menu_small->subtitle_style = subtitle_style;

    return btn_menu_small;
}

void free_btn_menu_small(btn_menu_small_t btn_menu_small) {
    free(btn_menu_small->title_style);
    free(btn_menu_small->subtitle_style);
    free(btn_menu_small);
}

btn_menu_big_t btn_menu_big_create(lv_obj_t *parent) {
    lv_obj_t *btn = btn_container_create(parent);
    lv_obj_set_size(btn, 400, 100);
    lv_obj_set_pos(btn, 39, 175);

    lv_obj_t *title_label = lv_label_create(btn);
    lv_label_set_text(title_label, "HISTORICO DE TORRA");
    lv_obj_set_pos(title_label, 87, 38);

    lv_style_t *title_style = (lv_style_t *)malloc(sizeof(lv_style_t));
    lv_style_init(title_style);
    lv_style_set_text_font(title_style, &lv_font_montserrat_20);
    lv_obj_add_style(title_label, title_style, 0);

    btn_menu_big_t btn_menu_big = (btn_menu_big_t)malloc(sizeof(s_btn_menu_big_t));
    btn_menu_big->btn = btn;
    btn_menu_big->title_style = title_style;

    return btn_menu_big;
}

void free_btn_menu_big(btn_menu_big_t btn_menu_big) {
    free(btn_menu_big->title_style);
    free(btn_menu_big);
}

void free_screen_menu(menu_obj_t menu_obj) {
    free_btn_menu_small(menu_obj->btn_menu_small[0]);
    free_btn_menu_small(menu_obj->btn_menu_small[1]);
    free_btn_menu_big(menu_obj->btn_menu_big);
    free(menu_obj);
}

void btn_manual_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    screen_manager_t screen_manager = (screen_manager_t)lv_event_get_user_data(e);

    if (code == LV_EVENT_CLICKED) {
        screen_manager_set_screen(screen_manager, MANUAL_MODE);
    }
}

void btn_auto_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked");
    } else if (code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled");
    }
}

void btn_history_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked");
    } else if (code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled");
    }
}