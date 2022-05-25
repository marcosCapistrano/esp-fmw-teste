#ifndef DATABASE_H
#define DATABASE_H

#include "esp_err.h"
#include "sqlite3.h"
#include "common_controller.h"

esp_err_t db_connection_create(sqlite3 **db);
void db_create_tables(sqlite3 **db);
void db_init(void);
void create_new_recipe(sqlite3 **db, const char *recipe_name, recipe_data_t recipe_data, sensor_data_t sensor_data);

#endif