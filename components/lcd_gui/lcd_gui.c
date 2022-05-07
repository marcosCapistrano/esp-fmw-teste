#include "lcd_gui.h"

#include <lvgl.h>
#include <lvgl_esp32_drivers/lvgl_helpers.h>
#include <stdio.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "time.h"

#include "common_params.h"
#include "menu.h"

#define LV_TICK_PERIOD_MS 1

static const char *TAG = "LCD_GUI";

static lv_disp_draw_buf_t disp_buf;
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;
static lv_indev_t *indev;

static void lv_tick_task(void *arg);

void lcd_gui_init() {
    lv_init();
    lvgl_driver_init();

    int32_t size_in_px = DISP_BUF_SIZE;
    lv_color_t *buf1 = heap_caps_malloc(size_in_px * sizeof(lv_color_t), MALLOC_CAP_DMA);

    lv_disp_draw_buf_init(&disp_buf, buf1, NULL, size_in_px);
    lv_disp_drv_init(&disp_drv);

    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.hor_res = LV_HOR_RES;
    disp_drv.ver_res = LV_VER_RES;
    lv_disp_drv_register(&disp_drv);

    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touch_driver_read;

    indev = lv_indev_drv_register(&indev_drv);

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "lv_tick_task",
    };

    static esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, pdMS_TO_TICKS(1)));
}

void lcd_gui_task(void *pvParameters) {
    controller_params_t params = (controller_params_t)pvParameters;
    QueueHandle_t input_control_queue = params->input_control_queue;
    QueueHandle_t output_notify_queue = params->output_notify_queue;  // Queue com os eventos que chegaram do controlador
    SemaphoreHandle_t xGuiSemaphore = xSemaphoreCreateMutex();

    lcd_gui_init();
    lv_obj_t *main_screen = lv_obj_create(lv_scr_act());
    static lv_obj_t *btn;
		btn = lv_btn_create(main_screen);
    lv_obj_set_size(btn, 180, 100);
    lv_obj_set_pos(btn, 40, 45);

    for (;;) {
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            lv_timer_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}



static void lv_tick_task(void *arg) {
    (void)arg;
    // ESP_LOGI(TAG, "Ticking...");

    lv_tick_inc(LV_TICK_PERIOD_MS);
}