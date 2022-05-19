#include "database.h"

#include "esp_log.h"
#include "esp_timer.h"

#include "sqlite3.h"
#include <stdio.h>

static const char *TAG = "DATABASE";

const char* data = "Callback function called";
static int callback(void *data, int argc, char **argv, char **azColName) {
   int i;
   printf("%s: ", (const char*)data);
   for (i = 0; i<argc; i++){
       printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

static int db_open(const char *filename, sqlite3 **db) {
    int rc = sqlite3_open(filename, db);

    if (rc) {
        ESP_LOGE(TAG, "Could not open database: %s\n", sqlite3_errmsg(*db));
        return rc;
    } else {
        ESP_LOGI(TAG, "Database Open SUCCESS!");
    }

    return rc;
}

char *zErrMsg = 0;
int db_exec(sqlite3 *db, const char *sql) {
    printf("%s\n", sql);
    int64_t start = esp_timer_get_time();
    int rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        printf("Operation done successfully\n");
    }
    printf("Time taken: %lld\n", esp_timer_get_time() - start);
    return rc;
}

void database_init(void) {
    sqlite3 *db1;
    int rc;

    sqlite3_initialize();

    if (db_open("/spiffs/test1.db", &db1))
        return;

    rc = db_exec(db1, "CREATE TABLE test1 (id INTEGER, content);");
    if (rc != SQLITE_OK) {
        sqlite3_close(db1);
        return;
    }

     rc = db_exec(db1, "INSERT INTO test1 VALUES (1, 'Hello, World from test1');");
    if (rc != SQLITE_OK) {
        sqlite3_close(db1);
        return;
    }

    rc = db_exec(db1, "SELECT * FROM test1");
    if (rc != SQLITE_OK) {
        sqlite3_close(db1);
        return;
    }

    sqlite3_close(db1);
}
