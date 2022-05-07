#ifndef LCD_COMPONENTS_H
#define LCD_COMPONENTS_H

#include "lvgl.h"

lv_obj_t* container_create(lv_obj_t *parent);
lv_obj_t* header_create(lv_obj_t *parent, const char* title); 

#endif