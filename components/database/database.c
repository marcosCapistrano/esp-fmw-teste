#include <stdio.h>
#include "database.h"

static const char* TAG = "DATABASE";

static int db_open(const char*filename, sqlite3 **db) {
	int rc = sqlite3_open(filename, db);

	if(rc) {
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
   int rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
   if (rc != SQLITE_OK) {
       printf("SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
   } else {
       printf("Operation done successfully\n");
   }
   printf("Time taken: %lld\n", esp_timer_get_time()-start);
   return rc;
}

void func(void)
{

}
