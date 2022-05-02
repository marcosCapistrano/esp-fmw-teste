#include "lcd_gui.h"

#include <lvgl.h>
#include <lvgl_esp32_drivers/lvgl_helpers.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define AUSYX_HOR_RES 480
#define AUSYX_VER_RES 320
#define LV_TICK_PERIOD_MS 1

static lv_disp_draw_buf_t disp_buf;
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;
static lv_indev_t *indev;

SemaphoreHandle_t xGuiSemaphore;

static void lv_tick_task(void* arg);

void lcd_gui_init(void) {
    xGuiSemaphore = xSemaphoreCreateMutex();
    lv_init();
    lvgl_driver_init();

    static lv_color_t buf_1[DISP_BUF_SIZE * 10];
    static lv_color_t buf_2[DISP_BUF_SIZE * 10];
    lv_disp_draw_buf_init(&disp_buf, buf_1, buf_2, DISP_BUF_SIZE * 10);

    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.hor_res = AUSYX_HOR_RES;
    disp_drv.ver_res = AUSYX_VER_RES;
    lv_disp_drv_register(&disp_drv);

    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touch_driver_read;
    indev = lv_indev_drv_register(&indev_drv);

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "lv_tick_task",
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));
}

void lcd_gui_task(void* params) {
    for(;;) {
        vTaskDelay(pdMS_TO_TICKS(5));

        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
						lv_timer_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
    }
}

static void lv_tick_task(void* arg) {
    (void)arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}