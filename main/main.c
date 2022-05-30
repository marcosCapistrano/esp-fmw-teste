#include "common.h"
#include "controller.h"
#include "database.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "lcd_gui.h"
#include "nvs_flash.h"

static const char* TAG = "MAIN";

static controller_t controller;
static lcd_gui_t lcd_gui;

static QueueHandle_t incoming_queue_commands;
static QueueHandle_t outgoing_queue_lcd;

static QueueHandle_t event_broadcast_queue;  // Queue que será lida pelo LCD e servidor sobre eventos no controlador
static EventGroupHandle_t event_group_sync;  // Event group usado para sincronizar as tasks do LCD e servidor terem recebido notificações do controlador

void app_main(void) {
    esp_err_t nvs = nvs_flash_init();
    if (nvs == ESP_ERR_NVS_NO_FREE_PAGES || nvs == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvs = nvs_flash_init();
    }
    ESP_ERROR_CHECK(nvs);

    // database_t db = db_init();

    incoming_queue_commands = xQueueCreate(10, sizeof(incoming_data_t));
    outgoing_queue_lcd = xQueueCreate(5, sizeof(outgoing_data_t));

    event_group_sync = xEventGroupCreate();
    event_broadcast_queue = xQueueCreate(10, sizeof(controller_event_t));

    controller = controller_init(incoming_queue_commands, outgoing_queue_lcd, event_broadcast_queue);
    lcd_gui = lcd_gui_init(controller->controller_data, incoming_queue_commands, outgoing_queue_lcd, event_broadcast_queue, event_group_sync);

    xTaskCreatePinnedToCore(lcd_gui_draw_task, "LCD_GUI_DRAW_TASK", 12000, lcd_gui, 5, NULL, 1);
    // xTaskCreatePinnedToCore(lcd_gui_update_task, "LCD_GUI_UPDATE_TASK", 18000, lcd_gui, 3, NULL, 1);
    xTaskCreatePinnedToCore(controller_task, "TORRADOR_CONTROLLER_TASK", 12000, controller, 2, NULL, 1);
}