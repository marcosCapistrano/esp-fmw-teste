#include "controller.h"
#include "database.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "lcd_gui.h"
#include "nvs_flash.h"
#include "sqlite3.h"

static const char* TAG = "MAIN";

static controller_t controller;
static lcd_gui_t lcd_gui;

static QueueHandle_t incoming_queue_commands;
static QueueHandle_t outgoing_queue_lcd;

void app_main(void) {
    esp_err_t nvs = nvs_flash_init();
    if (nvs == ESP_ERR_NVS_NO_FREE_PAGES || nvs == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvs = nvs_flash_init();
    }
    ESP_ERROR_CHECK(nvs);

    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        // .partition_label = "database",
        .partition_label = "storage",
        .max_files = 5,
        .format_if_mount_failed = true};

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    FILE *fp;
    fp = fopen("/spiffs/ausyx.db", "w");
    fclose(fp);

    sqlite3 *test_conn;
    db_init();
    db_connection_create(&test_conn);
    db_create_tables(&test_conn);
    // create_new_recipe(&test_conn, "TESTE", NULL, NULL);
    ESP_LOGI("INIT DB", "FINISHED");

    incoming_queue_commands = xQueueCreate(10, sizeof(incoming_data_t));
    outgoing_queue_lcd = xQueueCreate(5, sizeof(outgoing_data_t));

    controller = controller_init(incoming_queue_commands, outgoing_queue_lcd);
    lcd_gui = lcd_gui_init(controller->controller_data, incoming_queue_commands, outgoing_queue_lcd);

    // xTaskCreatePinnedToCore(lcd_gui_draw_task, "LCD_GUI_DRAW_TASK", 12000, lcd_gui, 1, NULL, 1);
    // xTaskCreatePinnedToCore(lcd_gui_update_task, "LCD_GUI_UPDATE_TASK", 18000, lcd_gui, 5, NULL, 1);
    // xTaskCreatePinnedToCore(controller_task, "TORRADOR_CONTROLLER_TASK", 12000, controller, 5, NULL, 1);
    return;
}