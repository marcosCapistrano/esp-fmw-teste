#include "lcd_gui.h"

#include <lvgl.h>
#include <lvgl_esp32_drivers/lvgl_helpers.h>

#include "common.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "screen_manager.h"

#define AUSYX_HOR_RES 480
#define AUSYX_VER_RES 320
#define LV_TICK_PERIOD_MS 10

static const char *TAG = "LCD_GUI";

static lv_disp_draw_buf_t disp_buf;
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;
static lv_indev_t *indev;
static esp_timer_handle_t periodic_timer;

static void lv_tick_task(void *arg);

lcd_gui_t lcd_gui_init(controller_data_t controller_data, QueueHandle_t incoming_queue_commands, QueueHandle_t outgoing_queue_lcd, QueueHandle_t event_broadcast_queue, EventGroupHandle_t event_group_sync) {
    lcd_gui_t lcd_gui = malloc(sizeof(s_lcd_gui_t));

    lcd_gui->event_group_sync = event_group_sync;
    lcd_gui->event_broadcast_queue = event_broadcast_queue;

    lv_init();
    lvgl_driver_init();

    int32_t size_in_px = DISP_BUF_SIZE;
    lv_color_t *buf1 = heap_caps_malloc(size_in_px * sizeof(lv_color_t), MALLOC_CAP_DMA);

    lv_disp_draw_buf_init(&disp_buf, buf1, NULL, size_in_px);

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

    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, pdMS_TO_TICKS(LV_TICK_PERIOD_MS)));

    lcd_gui->screen_manager = screen_manager_init(controller_data, incoming_queue_commands, outgoing_queue_lcd);

    return lcd_gui;
}

// void lcd_gui_update_task(void *pvParameters) {
//     lcd_gui_t lcd_gui = (lcd_gui_t)pvParameters;

//     screen_manager_t screen_manager = lcd_gui->screen_manager;
//     controller_data_t controller_data = lcd_gui->controller_data;

//     EventGroupHandle_t event_group_sync = lcd_gui->event_group_sync;
//     QueueHandle_t event_broadcast_queue = lcd_gui->event_broadcast_queue;

//     for (;;) {
//         ESP_LOGI(TAG, "Update Task..");

//         xEventGroupSync(event_group_sync, SERVER_EVENT_BIT, SYNC_BITS, pdTICKS_TO_MS(150));

//         vTaskDelay(pdMS_TO_TICKS(350));
//     }
// }

void lcd_gui_draw_task(void *pvParameters) {
    lcd_gui_t lcd_gui = (lcd_gui_t)pvParameters;
    SemaphoreHandle_t xGuiSemaphore = xSemaphoreCreateMutex();

    EventGroupHandle_t event_group_sync = lcd_gui->event_group_sync;
    QueueHandle_t event_broadcast_queue = lcd_gui->event_broadcast_queue;

    controller_event_t incoming_event;

    for (;;) {
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            lv_timer_handler();
            xSemaphoreGive(xGuiSemaphore);
        }

        if (xQueuePeek(event_broadcast_queue, &incoming_event, 0) == pdPASS) {
            screen_manager_update(lcd_gui->screen_manager, incoming_event);
            xEventGroupSync(event_group_sync, LCD_EVENT_BIT, LCD_EVENT_BIT, portMAX_DELAY);

            xQueueReceive(event_broadcast_queue, &incoming_event, 0); //Remove da queue..
        }

        vTaskDelay(pdMS_TO_TICKS(LV_TICK_PERIOD_MS));
    }
}

static void lv_tick_task(void *arg) {
    (void)arg;
    // ESP_LOGI(TAG, "Ticking...");

    lv_tick_inc(LV_TICK_PERIOD_MS);
}