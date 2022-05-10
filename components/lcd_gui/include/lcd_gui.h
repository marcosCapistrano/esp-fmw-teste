#ifndef LCG_GUI_H
#define LCG_GUI_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "screen.h"
#include "esp_timer.h"

typedef struct s_lcd_gui_draw_params_t {
    QueueHandle_t input_control_queue;
    QueueHandle_t output_notify_queue;
} s_lcd_gui_draw_params_t;

typedef struct s_lcd_gui_draw_params_t *lcd_gui_draw_params_t;

typedef struct s_lcd_gui_update_params_t {
    QueueHandle_t input_control_queue;
    QueueHandle_t output_notify_queue;
    controller_t controller;
} s_lcd_gui_update_params_t;

typedef struct s_lcd_gui_update_params_t *lcd_gui_update_params_t;

void lcd_gui_init(void);
void lcd_gui_draw_task(void* params);
void lcd_gui_update_task(void* params);

#endif