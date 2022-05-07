#include "components.h"

#include "lvgl.h"

lv_obj_t* container_create(lv_obj_t *parent) {
    lv_obj_t *container = lv_btn_create(parent);
    lv_obj_set_style_pad_all(container, 0, 0);
		lv_obj_set_style_border_width(container, 0, 0);
		lv_obj_set_style_outline_width(container, 0, 0);

		return container;
}

lv_obj_t* header_create(lv_obj_t *parent, const char* title) {
		lv_obj_t *header = container_create(parent);
		lv_obj_set_size(header, lv_obj_get_width(parent), 50);

		lv_obj_t *title_label = lv_label_create(header);
		lv_label_set_text(title_label, title);

		lv_obj_set_style_text_font(title_label, &lv_font_montserrat_20, 0);
		lv_obj_center(title_label);

		return header;
}