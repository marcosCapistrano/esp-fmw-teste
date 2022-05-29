#ifndef DATABASE_H
#define DATABASE_H

#include "esp_err.h"
#include "esp_spiffs.h"
#include "common_controller.h"

typedef struct s_database_t *database_t;
typedef struct s_database_t {
	size_t free_space;
	esp_vfs_spiffs_conf_t spiffs_conf;
} s_database_t;

database_t db_init(void);


#endif