#include "client_manifest.h"
#include "client_db.h"
//#include "client_downloader.h"


char * get_val(key_val_db_s * db, char * key)

{
  for(int i = 0; i <= db->count;i++)
    {
      if(!strcmp(db->arr[i].key,key)) return db->arr[i].value;
    }
  return NULL;
}


void manifest_processing(key_val_db_s *message)
{
  sqlite3 *db = open_voc_db();
  get_voc_access_vals(db,message);
  close_voc_db(db);
}

int install_cache(char * json)
{
  sqlite3 *db = open_voc_db();

  json_object * jobj = json_tokener_parse(json);
  if(jobj)
    {
      creat_cache_table_entry(db, json); //this loads json into db send json array obj
    }
  close_voc_db(db);
  return 0;
}
