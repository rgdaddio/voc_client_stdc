#include "stdio.h"
#include <sqlite3.h>
#include "json/json.h"
#include "json/json_object.h"
#include "client_json.h"
#include "client_manifest.h"

static void insertKeyValue(key_val_db_s * db,char * key, char * value)
{
    if(db->count >= MAX_SIZE)
      {
	printf("key value DB size %d exceeded %d\n", db->count,MAX_SIZE);
	return;
      }
    db->arr[db->count].key = key;
    db->arr[db->count].value = value;
    db->count++;
}

sqlite3 *open_voc_db(void)
{
  sqlite3 *db;
  //char *zErrMsg = 0;
  int rc;

  rc = sqlite3_open("voc_client.db", &db);

  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    return NULL;
  }else{
    fprintf(stderr, "Opened database successfully\n");
    return db;
  }
  return NULL;
}
void close_voc_db(sqlite3 *db)
{
  sqlite3_close(db);
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
  int i;
  //if(NotUsed)
  //std::map<std::string, std::string> used = *static_cast <std::map<std::string, std::string> *>(NotUsed);

  for(i=0; i<argc; i++){
    //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    if(NotUsed)
      {
	key_val_db_s * db = (key_val_db_s *)NotUsed;
	insertKeyValue(db,azColName[i], argv[i] ? argv[i] : "NULL");
      }
  }
  return 0;
}

int create_table(sqlite3 *db, const char *sql_stmt, const char *table)
{

  int rc;
  char *zErrMsg = 0;

  rc = sqlite3_exec(db, sql_stmt, callback, 0, &zErrMsg);
  if(rc != SQLITE_OK){
    fprintf(stdout, "sqllite table: %s error\n", table);
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }else{
    fprintf(stdout, "sqllite table: %s created sucessfully\n", table);
  }
  return 0;
}

int execute_query(sqlite3 *db, const char *sql_stmt, char * table)
{

  int rc;
  char *zErrMsg = 0;

  rc = sqlite3_exec(db, sql_stmt, callback, 0, &zErrMsg);
  if(rc != SQLITE_OK){
    fprintf(stdout, "sqllite table: %s error\n", table);
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }else{
    fprintf(stdout, "sqllite query: %s created sucess\n", table);
  }
  return 0;
}

int create_user_table(sqlite3 *db)
{
  const char *table = "voc_user";
  const char *sql = "create table if not exists voc_user ("\
    "userid text,"\
    "password text,"\
    "device_id text,"\
    "platform text,"\
    "device_type text,"\
    "access_token text,"\
    "refresh_token text,"\
    "voc_id text,"\
    "congestion_detection text,"\
    "ads_frequency text,"\
    "daily_quota integer,"\
    "daily_manifest integer,"\
    "daily_download_wifi integer,"\
    "daily_download_cellular integer,"\
    "congestion text,"		      \
    "sdk_capabilities text,"\
    "max_content_duration integer,"\
    "play_ads text,"\
    "skip_policy_first_time text,"\
    "tod_policy text,"\
    "token_expiration integer,"\
    "server text,"\
    "server_state text,"\
    "my_row integer primary key autoincrement);";

  create_table(db, sql, table);
  return 0;
}

int create_provider_table(sqlite3 *db)
{
  const char *table = "provider";
  const char *sql = "create table if not exists provider ("\
    "name text unique,"\
    "contentprovider text,"\
    "subscribed integer);";

  create_table(db, sql, table);
  return 0;
}

int create_category_table(sqlite3 *db)
{
  const char *table = "category";
  const char *sql = "create table if not exists category ("\
    "name text unique,"\
    "subscribed integer);";

  create_table(db, sql, table);
  return 0;
}

int create_uuid_table(sqlite3 *db)
{
    const char *table = "uuid_table";
    const char *sql = "create table if not exists uuid_table ("\
      "uuid text);";
  create_table(db, sql, table);
  return 0;
}

int create_playing_table(sqlite3 *db)
{
    const char *table = "playing";
    const char *sql = "create table if not exists playing ("\
      "unique_Id text,"\
      "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);";

    create_table(db, sql, table);
    return 0;
}

int create_content_table(sqlite3 *db)
{
  const char *table = "content_status";
  const char *sql = "create table if not exists content_status ("\
    "download_time text,"\
    "download length integer,"\
    "download_duration real,"\
    "eviction_info text,"\
    "user_rating int,"\
    "unique_id text,"\
    "my_row integer primary key autoincrement);";

  create_table(db, sql, table);
  return 0;
}

int create_consumption_table(sqlite3 *db)
{
  const char *table = "consumption_status";
  const char *sql = "create table if not exists consumption_status ("\
    "watch_time int,"\
    "watchstart integer,"\
    "watchend int,"\
    "my_row integer primary key autoincrement);";

  create_table(db, sql, table);
  return 0;
}

int create_ad_consumption_table(sqlite3 *db)
{
  const char *table = "ad_consumption_status";
  const char *sql = "create table if not exists ad_consumption_status ("\
    "adurl text,"\
    "duration int,"\
    "starttime integer,"\
    "stopposition int,"\
    "clicked int,"\
    "unique_id text,"\
    "my_row integer primary key autoincrement);";

  create_table(db, sql, table);
  return 0;
}

int create_cache_table(sqlite3 *db)
{
  const char *table = "cache_manifest";
  const char *sql = "create table if not exists cache_manifest ("\
    "local_file text,"\
    "local_thumbnail text,"\
    "local_nfo text,"\
    "video_size integer,"\
    "thumbnail_size integer,"\
    "download_date integer,"\
    "content_provider text,"\
    "category text,"\
    "unique_id text,"\
    "summary text,"\
    "title text,"\
    "duration integer,"\
    "timestamp integer,"\
    "sdk_metadata text,"\
    "streams text,"		      \
    "ad_server_url text,"\
    "tags text,"\
    "priority integer,"\
    "object_type text,"\
    "thumb_attrs text,"\
    "object_attrs text,"\
    "children text,"\
    "policy_name text,"\
    "key_server_url text,"\
    "save integer default 0,"\
    "my_row integer primary key autoincrement);";

  create_table(db, sql, table);
  return 0;
}

int insert_voc_table(sqlite3 *db, char *sql_stmt)
{
  int rc;
  char *zErrMsg = 0;
  rc = sqlite3_exec(db, sql_stmt, callback, 0, &zErrMsg);
  if(rc != SQLITE_OK){
    fprintf(stderr, "SQL insert error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }else{
    fprintf(stdout, "User table insert sucess\n");
  }
  return 0;
}

int select_voc_table(sqlite3 *db, char *sql_stmt, bool ret)
{
  int rc;
  char *zErrMsg = 0;

  static key_val_db_s value_map;
  value_map.count = 0;

  if(ret == False){
    rc = sqlite3_exec(db, sql_stmt, callback, 0, &zErrMsg);
  }
  else
    rc = sqlite3_exec(db, sql_stmt, callback, &value_map, &zErrMsg);

  if(rc != SQLITE_OK){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }else{
    fprintf(stdout, "User table select sucess\n");
  }

  if(value_map.count == 0)
    return 1;
  else
    return 0;
}


void select_voc_table_json(sqlite3 *db, char * sql_stmt, key_val_db_s * value_map)
{
  int rc;
  char *zErrMsg = 0;

  value_map->count = 0;

  rc = sqlite3_exec(db, sql_stmt, callback, value_map, &zErrMsg);

  if(rc != SQLITE_OK){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }else{
    fprintf(stdout, "User table select sucess\n");
  }

}

int create_user_table_entry(sqlite3 *db, char * jstr, char * server)
{

  json_object *new_obj = json_tokener_parse(jstr);
  if(!new_obj)
    {
      printf("\nJSON problem\n\n");
      exit(-1);
    }

  char * userId = "test";
  char * password = "test";
  char * tnull = "NULL";

  char sqlstatement[4096];
  sprintf(sqlstatement,
	  "%s%s,'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',%d,%d,%ld,'%s',%d,'%s','%s','%s',%ld,'%s','%s');",
	  "INSERT INTO voc_user (my_row, userid, password, device_id, platform, device_type, access_token, refresh_token, voc_id, congestion_detection, ads_frequency, daily_quota, daily_manifest, daily_download_wifi, daily_download_cellular, sdk_capabilities, max_content_duration, play_ads, skip_policy_first_time, tod_policy, token_expiration, server, server_state) VALUES (",
	  tnull,
	  userId,
	  password,
	  get_device_id(new_obj),
	  get_platform(new_obj),
	  get_device_type(new_obj),
	  get_access_token(new_obj),
	  get_refresh_token(new_obj),
	  get_voc_id(new_obj),
	  get_congestion_detection(new_obj),
	  get_ads_frequency(new_obj),
	  get_daily_download_quota(new_obj),
	  get_daily_download_manifest(new_obj),
	  get_daily_download_wifi(new_obj),
	  get_daily_download_cellular(new_obj),
	  get_sdk_capabilities(new_obj),
	  get_max_content_duration(new_obj),
	  get_play_ads(new_obj),
	  get_skip_policy(new_obj),
	  get_tod_policy(new_obj),
	  get_token_expiration(new_obj),
	  server,
	  get_server_state(new_obj)
	  );
  insert_voc_table(db, sqlstatement);
  //std::cout << "sql stmt: " << sqlstatement << std::endl;
  return 0;
}

int creat_cache_table_entry(sqlite3 *db, char * jstr)
{
  // to do append only one in each sprintf
  char * tnull = "NULL";

  json_object *new_obj = json_tokener_parse(jstr); //the manifest is a json array

  static char sqlstatement[16000];
  int len = sprintf(sqlstatement,"INSERT INTO cache_manifest (my_row, local_file, local_thumbnail, local_nfo, video_size, thumbnail_size, download_date, content_provider, category, unique_id, summary, title, duration,timestamp, sdk_metadata, streams, ad_server_url, tags, priority, object_type, thumb_attrs, object_attrs, children, policy_name, key_server_url) VALUES(");

  len += sprintf(sqlstatement+len,"%s,\"%s\",\"%s\",\"%s\"",
		 tnull,
		 get_local_file(new_obj),
		 get_local_thumb_file(new_obj),
		 get_local_nfo(new_obj)
		 );
  len += sprintf(sqlstatement+len,",\"%s\",\"%d\",\"%zd\"",
		 get_local_file_size(new_obj),
		 get_thumb_size(new_obj),
		 get_download_time()
		 );
  len += sprintf(sqlstatement+len,",\"%s\",\"%s\",\"%s\"",
		 get_content_provider(new_obj),                                                                                                                                                                  	  get_category(new_obj),                                                                                                                                                                          	   get_unique_id(new_obj)
		 );
  len += sprintf(sqlstatement+len,",'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s');",
		 quotesql(get_summary(new_obj)),
		 get_title(new_obj),
		 get_duration(new_obj),
		 get_time_stamp(new_obj),
		 get_sdk_metadata(new_obj),
		 get_streams(new_obj),
		 get_adserver_url(new_obj),
		 get_tags(new_obj),
		 get_priority(new_obj),
		 get_object_type(new_obj),
		 get_thumb_attribs(new_obj),
		 get_object_attribs(new_obj),
		 get_children(new_obj),
		 get_policy_name(new_obj),
		 get_key_server_url(new_obj)
		 );
  #if 0

  sprintf(sqlstatement,"%s%s,%s,%s,%s,%d,%zd,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s);"
    "INSERT INTO cache_manifest (my_row, local_file, local_thumbnail, local_nfo, video_size, thumbnail_size, download_date, content_provider, category, unique_id, summary, title, duration,\
timestamp, sdk_metadata, streams, ad_server_url, tags, priority, object_type, thumb_attrs, object_attrs, children, policy_name, key_server_url) VALUES\
 (",
	  tnull,
	  get_local_file(new_obj),
	  get_local_thumb_file(new_obj),
	  get_local_nfo(new_obj),
	  get_local_file_size(new_obj),
	  get_thumb_size(new_obj),
	  get_download_time(),
	  get_content_provider(new_obj),
	  get_category(new_obj),
	  get_unique_id(new_obj),
	  get_summary(new_obj),
	  get_title(new_obj),
	  get_duration(new_obj),
	  get_time_stamp(new_obj),
	  get_sdk_metadata(new_obj),
	  get_streams(new_obj),
	  get_adserver_url(new_obj),
	  get_tags(new_obj),
	  get_priority(new_obj),
	  get_object_type(new_obj),
	  get_thumb_attribs(new_obj),
	  get_object_attribs(new_obj),
	  get_children(new_obj),
	  get_policy_name(new_obj),
	  get_key_server_url(new_obj)
	  );
  #endif

  insert_voc_table(db, sqlstatement);
  return 0;
}


char * get_voc_user_json(sqlite3 *db, char * stmt)
{

  int rc = 0;
  char * json = "test";
  char * ustmt = "select device_id, platform, voc_id, access_token, refresh_token, server, server_state from voc_user";
  //std::string dstmt = "DELETE from voc_user where my_row='1';";
  rc = select_voc_table(db, stmt,False);

  if(rc == -1)
    {
      //std::cout << "table value1: " << rc << std::endl;
      return json;
    }

  rc = select_voc_table(db, ustmt, True);
  //if(rc)
  //json.clear();
  return json;
}

void get_voc_access_vals(sqlite3 *db,key_val_db_s *value_map)
{
  char * ustmt = "select device_id, platform, voc_id, access_token, refresh_token, server, server_state from voc_user";
  select_voc_table_json(db, ustmt,value_map);
  //printf("%s count %d\n",__FUNCTION__,value_map->count);

}
