#ifndef LCG_GUI_H
#define LCG_GUI_H

#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "screen_manager.h"

typedef struct s_lcd_gui_t {
    lv_disp_draw_buf_t *disp_buf;
    lv_disp_drv_t *disp_drv;
    lv_indev_drv_t *indev_drv;
    lv_indev_t* indev;
    esp_timer_handle_t periodic_timer;

    screen_manager_t screen_manager;
} s_lcd_gui_t;

typedef struct s_lcd_gui_t *lcd_gui_t;

lcd_gui_t lcd_gui_init(controller_data_t controller_data, QueueHandle_t incoming_queue_commands, QueueHandle_t outgoing_queue_lcd);
void lcd_gui_draw_task(void* params);
void lcd_gui_update_task(void* params);

#endif