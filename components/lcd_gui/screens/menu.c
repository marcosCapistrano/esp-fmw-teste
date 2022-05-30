#include "menu.h"

#include "screen_manager.h"
#include "screen.h"
#include "components.h"
#include "lvgl.h"

screen_menu_t screen_menu_create(lv_obj_t *main_screen, screen_manager_t screen_manager) {
    screen_menu_t screen_menu = (screen_menu_t)malloc(sizeof(s_screen_menu_t));

    btn_menu_small_create(main_screen, 40, 45, "MANUAL", 44, 45, &btn_manual_event_handler, screen_manager);
    btn_menu_small_create(main_screen, 260, 45, "AUTOMATICO", 21, 45, &btn_manual_event_handler, screen_manager);
    btn_menu_big_create(main_screen);

    return screen_menu;
}

void menu_update(screen_menu_t screen_menu) {
}

void menu_delete(screen_menu_t screen_menu) {
    free(screen_menu);
}

screen_i menu_as_screen = &(s_screen_i){
    .update = (void *)menu_update,
    .delete = (void *)menu_delete};

void btn_menu_small_create(lv_obj_t *parent, int x, int y, const char *subtitle, int subtitle_x, int subtitle_y, void *event_handler, screen_manager_t screen_manager) {
    lv_obj_t *btn = btn_container_create(parent);
    lv_obj_set_size(btn, 180, 100);
    lv_obj_set_pos(btn, x, y);

    lv_obj_t *title_label = lv_label_create(btn);
    lv_label_set_text(title_label, "MODO");
    lv_obj_set_pos(title_label, 69, 31);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_12, 0);

    lv_obj_t *subtitle_label = lv_label_create(btn);
    lv_label_set_text(subtitle_label, subtitle);
    lv_obj_set_pos(subtitle_label, subtitle_x, subtitle_y);
    lv_obj_set_style_text_font(subtitle_label, &lv_font_montserrat_20, 0);

    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, screen_manager);
}

void btn_menu_big_create(lv_obj_t *parent) {
    lv_obj_t *btn = btn_container_create(parent);
    lv_obj_set_size(btn, 400, 100);
    lv_obj_set_pos(btn, 39, 175);

    lv_obj_t *title_label = lv_label_create(btn);
    lv_label_set_text(title_label, "HISTORICO DE TORRA");
    lv_obj_set_pos(title_label, 87, 38);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_20, 0);
}

void btn_manual_event_handler(lv_event_t *e) {
    screen_manager_t screen_manager = (screen_manager_t)lv_event_get_user_data(e);

    screen_manager_set_screen(screen_manager, MANUAL);
    // if (code == LV_EVENT_CLICKED) {
    //     screen_manager_set_screen(screen_manager, MANUAL_MODE);
    // }
}

void btn_auto_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);

    // if (code == LV_EVENT_CLICKED) {
    //     LV_LOG_USER("Clicked");
    // } else if (code == LV_EVENT_VALUE_CHANGED) {
    //     LV_LOG_USER("Toggled");
}

void btn_history_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);

    // if (code == LV_EVENT_CLICKED) {
    //     LV_LOG_USER("Clicked");
    // } else if (code == LV_EVENT_VALUE_CHANGED) {
    //     LV_LOG_USER("Toggled");
    // }
}