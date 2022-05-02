#include "database_controller.h"

#include <stdio.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_timer.h"
#include "sqlite3.h"

static const char *TAG = "DATABASE_CONTROLLER";

struct database_controller_instance_t {
    sqlite3 *db;
};

esp_err_t db_open(const char *filename, sqlite3 **db) {
    int rc = sqlite3_open(filename, db);
    if (rc) {
        ESP_LOGE(TAG, "Cant open database: %s\n", sqlite3_errmsg(*db));
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "Open database OK");
    return ESP_OK;
}

// esp_err_t db_exec(sqlite3 *db, const char *sql) {
//     char *exec_err_msg = 0;
//     int rc = sqlite3_exec(db, sql, callback, (void *)data, &db_exec_err_msg);
//     if (rc != SQLITE_OK) {
//         ESP_LOGE(TAG, "SQL Error: %s\n", db_exec_err_msg);
//         return ESP_FAIL;
//     }

//     ESP_LOGD(TAG, "Exec Operation OK");
//     return ESP_OK;
// }
database_controller_t *database_controller_create() {
    sqlite3 *db;

    ESP_LOGI(TAG, "Creating database controller...");
		ESP_LOGI(TAG, "Initializing SPIFFS...");

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

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s). Formatting...", esp_err_to_name(ret));
        esp_spiffs_format(conf.partition_label);
        return NULL;
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

	ESP_LOGI(TAG, "Initializing Database...");
	sqlite3_initialize();

	if(db_open("/spiffs/ausyx.db", &db) != ESP_OK) {
		return NULL;
	}

	database_controller_t *database_controller = malloc(sizeof(database_controller_t));
	database_controller->db = db;

	return database_controller;
}
