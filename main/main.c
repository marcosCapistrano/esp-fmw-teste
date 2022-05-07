#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_params.h"
#include "controller.h"
#include "database_controller.h"
#include "driver/gpio.h"
#include "esp_freertos_hooks.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lcd_gui.h"
#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
#include "nvs_flash.h"
#include "web_server.h"
#include "wifi_connect.h"

static const char* TAG = "MAIN";

static QueueHandle_t stage_control_queue;
static QueueHandle_t input_control_queue;

static QueueHandle_t stage_notify_queue;
static QueueHandle_t output_notify_queue;

static controller_params_t controller_params;

void app_main(void) {
    esp_err_t nvs = nvs_flash_init();
    if (nvs == ESP_ERR_NVS_NO_FREE_PAGES || nvs == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvs = nvs_flash_init();
    }
    ESP_ERROR_CHECK(nvs);

    // database_controller_t *database_controller = database_controller_create();

    input_control_queue = xQueueCreate(10, sizeof(input_event_t));
    output_notify_queue = xQueueCreate(1, sizeof(output_event_t));

    controller_params = controller_params_init(input_control_queue, output_notify_queue);

    // wifi_init();
    // web_server_init();

    // xTaskCreatePinnedToCore(wifi_task, "WIFI_TASK", 8192, NULL, 0, NULL, 0);
    // xTaskCreatePinnedToCore(controller_task, "TORRADOR_CONTROLLER_TASK", 2048, controller_params, 5, NULL, 1);
    // xTaskCreatePinnedToCore(web_server_task, "WEB_SERVER_TASK", 8192, torrador_controller_params, 2, NULL, 1);
    xTaskCreatePinnedToCore(lcd_gui_task, "LCD_GUI_TASK", 24000, controller_params, 5, NULL, 1);
    return;
}