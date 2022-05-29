#include "database.h"

#include <stdio.h>

#include "common_controller.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_timer.h"

static const char *TAG = "DATABASE";

size_t db_get_free_space(database_t db) {
    size_t total = 0, used = 0;
    esp_err_t ret = esp_spiffs_info(db->spiffs_conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    }

    return total * .75 - used;
}

database_t db_init(void) {
    ESP_LOGI(TAG, "Initializing Database SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "database",
        .max_files = 5,
        .format_if_mount_failed = true};

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return NULL;
    }

    database_t db = malloc(sizeof(s_database_t));
    db->spiffs_conf = conf;
    db->free_space = db_get_free_space(db);

    ESP_LOGI(TAG, "Database Partition Free Space: %d", db->free_space);

    return db;
}

// static char *array_to_string()

//     static char *recipe_data_to_string(recipe_data_t recipe_data) {
//     if (0 > asprintf(&sql, "INSERT INTO recipes (recipe_name) VALUES (%s);", recipe_name)) return;
// }