#include "stdio.h"
#include <sqlite3.h>
#include "json/json.h"
#include "json/json_object.h"
#include "client_manifest.h"
#include "client_db.h"
#include "client_json.h"

int handle_registration_json(char * jstr, char * server)
{
  sqlite3 *db = open_voc_db();

  int rc = create_user_table(db);
  if(rc == 0)
    {
      rc = create_uuid_table(db);
      rc = create_provider_table(db);
      rc = create_category_table(db);
      rc = create_cache_table(db);
      rc = create_content_table(db);
      rc = create_consumption_table(db);
      rc = create_ad_consumption_table(db);
      rc = create_playing_table(db);
      rc = create_user_table_entry(db, jstr, server);
      printf("DB regisistration complete\n");
    }
  close_voc_db(db);
  return 0;
}

int handle_provider_list(char * jstr, char * server)
{
  sqlite3 *db = open_voc_db();

  // change
  //char * upsert_query =  upsert_query_start + parse_provider_list(jstr);
  char * upsert_query =  "test";

  execute_query(db, upsert_query, "provider");
  return 0;
}

char * validate_user_for_registration(void)
{
  char * testtable = "SELECT name FROM sqlite_master WHERE type='table' AND name='voc_user'";
  sqlite3 *db = open_voc_db();
  char * json = "error";
  int rc = create_user_table(db);
  if(rc == 0)
    json  =  get_voc_user_json(db, testtable);
  close_voc_db(db);
  return json;
}
