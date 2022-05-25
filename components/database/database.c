#include "database.h"

#include <stdio.h>

#include "common_controller.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "sqlite3.h"

static const char *TAG = "DATABASE";

esp_err_t db_connection_create(sqlite3 **db) {
    int rc = sqlite3_open("/spiffs/ausyx.db", db);

    ESP_LOGE(TAG, "Conn create rc: %d", rc);
    if (rc) {
        ESP_LOGE(TAG, "Could not open database: %s\n", sqlite3_errmsg(*db));
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Database Open SUCCESS!");
    return ESP_OK;
}

esp_err_t db_exec(sqlite3 *db, const char *sql, sqlite3_stmt **stmt) {
    int prep_rc = sqlite3_prepare_v2(db, sql, -1, stmt, NULL);
    if (prep_rc != SQLITE_OK) {
        ESP_LOGE(TAG, "Error preparing statement: %s", sql);
        return ESP_FAIL;
    }

    int step_rc;
    // do {
    step_rc = sqlite3_step(*stmt);
    ESP_LOGE("Table creation rc: ", "%d", step_rc);
    // if (step_rc != SQLITE_OK) {
    //     ESP_LOGE(TAG, "Failed stepping statement: %s", sql);
    //     return ESP_FAIL;
    // }
    ESP_LOGI(TAG, "step: %d", step_rc);
    ESP_LOGE(TAG, "ext: %d", (int) sqlite3_extended_errcode(db));
    // } while (step_rc != SQLITE_DONE);

    int finalize_rc = sqlite3_finalize(*stmt);
    // if (finalize_rc != SQLITE_OK) {
    //     ESP_LOGE(TAG, "Failed finalizing statement: %s", sql);
    //     return ESP_FAIL;
    // }

    return ESP_OK;
}

void db_create_tables(sqlite3 **db) {
    const char *table_recipes_sql = "CREATE TABLE IF NOT EXISTS recipes (recipe_id INTEGER PRIMARY KEY AUTOINCREMENT, recipe_name TEXT NOT NULL UNIQUE);";
    sqlite3_stmt *table_recipe_stmt;
    ESP_ERROR_CHECK(db_exec(*db, table_recipes_sql, &table_recipe_stmt));

    const char *table_recipe_data_sql = "CREATE TABLE IF NOT EXISTS recipe_data (recipe_data_id INTEGER PRIMARY KEY AUTOINCREMENT, recipe_id INTEGER, pre_heating_temp INTEGER, array_potencia TEXT, array_cilindro TEXT, array_turbina TEXT, array_time TEXT, FOREIGN KEY(recipe_id) REFERENCES recipes(contact_id) ON DELETE CASCADE ON UPDATE NO ACTION);";
    sqlite3_stmt *table_recipe_data_stmt;
    ESP_ERROR_CHECK(db_exec(*db, table_recipe_data_sql, &table_recipe_data_stmt));

    const char *table_sensor_data_sql = "CREATE TABLE IF NOT EXISTS sensor_data (sensor_data_id INTEGER PRIMARY KEY AUTOINCREMENT, recipe_id INTEGER, history_id INTEGER, array_temp_ar TEXT, array_temp_grao TEXT, array_time TEXT, FOREIGN KEY(recipe_id) REFERENCES recipes(contact_id) ON DELETE CASCADE ON UPDATE NO ACTION FOREIGN KEY(history_id) REFERENCES history(history_id) ON DELETE CASCADE ON UPDATE NO ACTION);";
    sqlite3_stmt *table_sensor_data_stmt;
    ESP_ERROR_CHECK(db_exec(*db, table_sensor_data_sql, &table_sensor_data_stmt));

    const char *table_history_sql = "CREATE TABLE IF NOT EXISTS history (history_id INTEGER PRIMARY KEY AUTOINCREMENT);";
    sqlite3_stmt *table_history_stmt;
    ESP_ERROR_CHECK(db_exec(*db, table_history_sql, &table_history_stmt));

    ESP_LOGI(TAG, "Finalized table creation");
}

void create_new_recipe(sqlite3 **db, const char *recipe_name, recipe_data_t recipe_data, sensor_data_t sensor_data) {
    char *sql;
    if (0 > asprintf(&sql, "INSERT INTO recipes (recipe_name) VALUES (%s);", recipe_name)) return;
    sqlite3_stmt *recipe_create_stmt;

    sqlite3_prepare_v2(*db, sql, -1, &recipe_create_stmt, NULL);
    sqlite3_step(recipe_create_stmt);

    const int recipe_id = sqlite3_last_insert_rowid(*db);

    if (0 > asprintf(&sql, "INSERT INTO recipe_data (pre_heating_temp, array_potencia, array_cilindro, array_turbina, recipe_id VALUES ();")) return;
    sqlite3_stmt *recipe_data_create_stmt;

    sqlite3_prepare_v2(*db, sql, -1, &recipe_data_create_stmt, NULL);
    int step2_rc = sqlite3_step(recipe_data_create_stmt);
    ESP_LOGE(TAG, "Step:: %d", step2_rc);

    char *sql2 = "SELECT * FROM recipe_data";
    sqlite3_stmt *select_stmt;
    sqlite3_finalize(recipe_data_create_stmt);

    sqlite3_prepare_v2(*db, sql2, -1, &select_stmt, NULL);
    int step_rc = sqlite3_step(select_stmt);

    if (step_rc == SQLITE_ROW) {
        ESP_LOGE(TAG, "Column count: %d", (int)sqlite3_column_count(&select_stmt));
    } else {
        if (step_rc == SQLITE_MISUSE) {
            ESP_LOGE(TAG, "MISUSE!: %d", (int)step_rc);
        }
        ESP_LOGE(TAG, "not sqlite_row: %d", (int)step_rc);
    }

    sqlite3_finalize(select_stmt);
    free(sql);
}

void db_init(void) {
    // sqlite3_initialize();
}

// static char *array_to_string()

//     static char *recipe_data_to_string(recipe_data_t recipe_data) {
//     if (0 > asprintf(&sql, "INSERT INTO recipes (recipe_name) VALUES (%s);", recipe_name)) return;
// }