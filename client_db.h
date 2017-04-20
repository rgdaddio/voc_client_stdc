#ifndef _CLIENT_DB_H_
#define _CLIENT_DB_H_
#include "client_json.h"


extern sqlite3 *open_voc_db(void);
extern void close_voc_db(sqlite3 *db);
extern int create_table(sqlite3 *db, const char *sql_stmt, char * table);
extern int execute_query(sqlite3 *db, const char *sql_stmt, char * table);
extern int create_user_table(sqlite3 *db);
extern int insert_voc_table(sqlite3 *db, char * sql_stmt);
extern int select_voc_table(sqlite3 *db, char * sql_stmt, bool ret);
extern int create_user_table_entry(sqlite3 *db, char * jstr, char * server);
extern char * get_voc_user_json(sqlite3 *db, char * stmt);
extern int create_provider_table(sqlite3 *db);
extern int create_category_table(sqlite3 *db);
extern int create_uuid_table(sqlite3 *db);
extern int create_playing_table(sqlite3 *db);
extern int create_content_table(sqlite3 *db);
extern int create_consumption_table(sqlite3 *db);
extern int create_ad_consumption_table(sqlite3 *db);
extern int create_cache_table(sqlite3 *db);
extern int creat_cache_table_entry(sqlite3 *db, char * jstr);
void get_voc_access_vals(sqlite3 *db, key_val_db_s * kv);
#endif
