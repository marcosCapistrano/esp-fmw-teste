#ifndef DATABASE_H
#define DATABASE_H

#include "sqlite3.h"

int db_connection_create(sqlite3 **db);
void db_create_tables(sqlite3 **db);
void db_init(void);

#endif