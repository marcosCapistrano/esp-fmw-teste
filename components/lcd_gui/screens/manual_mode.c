#include "manual_mode.h"

#include "lvgl.h"
#include <lvgl_esp32_drivers/lvgl_helpers.h>

#include "components.h"

void screen_manual_mode_create(lv_obj_t* main_screen) {
	lv_obj_t *header = header_create(main_screen, "MODO MANUAL");
}

// static btn_menu_small_t btn_menu_small_create(lv_obj_t *parent, int x, int y, const char* subtitle, int subtitle_x, int subtitle_y) {
//     lv_obj_t *btn = container_create(parent);
//     lv_obj_set_size(btn, 180, 100);
//     lv_obj_set_pos(btn, x, y);

//     lv_obj_t *title_label = lv_label_create(btn);
// 		lv_label_set_text(title_label, "MODO");
// 		lv_obj_set_pos(title_label, 69, 31);

// 		lv_obj_t *subtitle_label = lv_label_create(btn);
// 		lv_label_set_text(subtitle_label, subtitle);
// 		lv_obj_set_pos(subtitle_label, subtitle_x, subtitle_y);

// 		lv_style_t *title_style = (lv_style_t *)malloc(sizeof(lv_style_t));
// 		lv_style_init(title_style);
// 		lv_style_set_text_font(title_style, &lv_font_montserrat_12);
// 		lv_obj_add_style(title_label, title_style, 0);

// 		lv_style_t *subtitle_style = (lv_style_t *)malloc(sizeof(lv_style_t));
// 		lv_style_init(subtitle_style);
// 		lv_style_set_text_font(subtitle_style, &lv_font_montserrat_20);
// 		lv_obj_add_style(subtitle_label, subtitle_style, 0);

// 		btn_menu_small_t btn_menu_small = (btn_menu_small_t)malloc(sizeof(s_btn_menu_small_t));
// 		// btn_menu_small->btn = btn;
// 		btn_menu_small->title_style = title_style;
// 		btn_menu_small->subtitle_style = subtitle_style;

// 		return btn_menu_small;
// }

// void free_btn_menu_small(btn_menu_small_t btn_menu_small) {
// 	free(btn_menu_small->btn);
// 	free(btn_menu_small->title_style);
// 	free(btn_menu_small->subtitle_style);
// 	free(btn_menu_small);
// }

// static btn_menu_big_t btn_menu_big_create(lv_obj_t *parent) {
//     lv_obj_t *btn = container_create(parent);
//     lv_obj_set_size(btn, 400, 100);
//     lv_obj_set_pos(btn, 39, 175);

//     lv_obj_t *title_label = lv_label_create(btn);
// 		lv_label_set_text(title_label, "HISTORICO DE TORRA");
// 		lv_obj_set_pos(title_label, 87, 38);

// 		lv_style_t *title_style = (lv_style_t *)malloc(sizeof(lv_style_t));
// 		lv_style_init(title_style);
// 		lv_style_set_text_font(title_style, &lv_font_montserrat_20);
// 		lv_obj_add_style(title_label, title_style, 0);

// 		btn_menu_big_t btn_menu_big = (btn_menu_big_t)malloc(sizeof(s_btn_menu_big_t));
// 		btn_menu_big->btn = btn;
// 		btn_menu_big->title_style = title_style;

// 		return btn_menu_big;
// }

// void free_btn_menu_big(btn_menu_big_t btn_menu_big) {
// 	free(btn_menu_big->btn);
// 	free(btn_menu_big->title_style);
// 	free(btn_menu_big);
// }
