#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_torrador_controller.h"
#include "database_controller.h"
#include "driver/gpio.h"
#include "esp_freertos_hooks.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
#include "nvs_flash.h"
#include "torrador_controller.h"
#include "web_server.h"
#include "wifi_connect.h"
#include "lcd_gui.h"
// #include "lcd_gui.h"

// static const char* TAG = "MAIN";

static QueueHandle_t state_event_queue;    // Queue "mailbox" usada pelo controlador para avisar IHM e web_server de mudanças
static QueueHandle_t control_event_queue;  // Queue usada pela IHM e pelo web_server para avisar o controlador de inputs

void app_main(void) {
    esp_err_t nvs = nvs_flash_init();
    if (nvs == ESP_ERR_NVS_NO_FREE_PAGES || nvs == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvs = nvs_flash_init();
    }
    ESP_ERROR_CHECK(nvs);

    // database_controller_t *database_controller = database_controller_create();

    state_event_queue = xQueueCreate(1, sizeof(torrador_state_t));
    control_event_queue = xQueueCreate(10, sizeof(control_event_t));
    torrador_controller_params_t *torrador_controller_params = torrador_controller_params_create(control_event_queue, state_event_queue);

    wifi_init();
    web_server_init();
    peripherals_init();

    xTaskCreatePinnedToCore(wifi_task, "WIFI_TASK", 8192, NULL, 0, NULL, 0);
    xTaskCreatePinnedToCore(torrador_controller_task, "TORRADOR_CONTROLLER_TASK", 2048, torrador_controller_params, 5, NULL, 1);
    // xTaskCreatePinnedToCore(web_server_task, "WEB_SERVER_TASK", 8192, torrador_controller_params, 2, NULL, 1);
    xTaskCreatePinnedToCore(lcd_gui_task, "LCD_GUI_TASK", 8192, torrador_controller_params, 5, NULL, 1);
}