#include <stdio.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "wifi_connect.h"
#include "web_server.h"
#include "torrador_controller.h"

static const char* TAG = "MAIN";

static QueueHandle_t controller_queue;

void app_main(void) {
    esp_err_t nvs = nvs_flash_init();
    if (nvs == ESP_ERR_NVS_NO_FREE_PAGES || nvs == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvs = nvs_flash_init();
    }
    ESP_ERROR_CHECK(nvs);

    controller_queue = xQueueCreate(3, sizeof(bool));

    wifi_init();
    web_server_init(controller_queue);
    peripherals_init();

    xTaskCreatePinnedToCore(wifi_task, "WIFI_TASK", 8192, NULL, 0, NULL, 0);
    xTaskCreatePinnedToCore(torrador_controller_task, "TORRADOR_CONTROLLER_TASK", 2048, controller_queue, 5, NULL, 1);
}
