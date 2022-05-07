#include "menu.h"

#include "lvgl.h"
#include <lvgl_esp32_drivers/lvgl_helpers.h>

static void btn_manual_create(lv_obj_t *parent);

void screen_menu_init(lv_obj_t *main_screen) {
	btn_manual_create(main_screen);
}

void btn_manual_create(lv_obj_t *parent) {
    static lv_obj_t *btn;
		btn = lv_btn_create(parent);
    lv_obj_set_size(btn, 180, 100);
    lv_obj_set_pos(btn, 40, 45);

    static lv_obj_t *title_label;
		title_label = lv_label_create(btn);
		lv_label_set_text(title_label, "MODO");
		lv_obj_set_pos(title_label, 31, 69);

		static lv_obj_t *subtitle_label;
		subtitle_label = lv_label_create(btn);
		lv_label_set_text(title_label, "MANUAL");
		lv_obj_set_pos(title_label, 45, 44);

		// static lv_style_t title_style;
		// lv_style_init(&title_style);
		// lv_style_set_text_font(&title_style, &lv_font_montserrat_12);
		// lv_obj_add_style(title_label, &title_style, 0);

		// static lv_style_t subtitle_style;
		// lv_style_init(&subtitle_style);
		// lv_style_set_text_font(&subtitle_style, &lv_font_montserrat_20);
		// lv_obj_add_style(subtitle_label, &subtitle_style, 0);

		// btn_manual_t btn_manual = (btn_manual_t)malloc(sizeof(s_btn_manual_t));
		// btn_manual->title_style = title_style;
		// btn_manual->subtitle_style = subtitle_style;

		// return btn_manual;
}

void free_btn_manual(btn_manual_t btn_manual) {
	free(btn_manual->title_style);
	free(btn_manual->subtitle_style);
	free(btn_manual);
}
