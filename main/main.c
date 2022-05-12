#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "nvs_flash.h"
#include "controller.h"
#include "lcd_gui.h"

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

    incoming_queue_commands = xQueueCreate(10, sizeof(s_incoming_data_t));
    outgoing_queue_lcd = xQueueCreate(5, sizeof(s_outgoing_data_t));

    controller = controller_init(incoming_queue_commands, outgoing_queue_lcd);
    lcd_gui = lcd_gui_init(controller->controller_data, incoming_queue_commands, outgoing_queue_lcd);

    xTaskCreatePinnedToCore(lcd_gui_draw_task, "LCD_GUI_DRAW_TASK", 12000, lcd_gui, 1, NULL, 1);
    xTaskCreatePinnedToCore(lcd_gui_update_task, "LCD_GUI_UPDATE_TASK", 12000, lcd_gui, 5, NULL, 1);
    xTaskCreatePinnedToCore(controller_task, "TORRADOR_CONTROLLER_TASK", 2048, controller, 5, NULL, 1);
    return;
}