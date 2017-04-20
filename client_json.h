#ifndef _CLIENT_JSON_H
#define _CLIENT_JSON_H

#include "stdio.h"
#include <sqlite3.h>
#include "json/json.h"
#include "json/json_object.h"

//#include "client.h"

#include <time.h>
enum bool_vals{
  False = 0,
  True
};
typedef int bool;

extern char * get_registration_json(void);

const char * get_voc_id(json_object *j);
char * get_platform(json_object *j);
char * get_device_id(json_object *j);
char * get_device_type(json_object *j);
const char * get_access_token(json_object *j);
const char * get_refresh_token(json_object *j);
const char * get_congestion_detection(json_object *j);
const char * get_ads_frequency(json_object *j);
const char * get_daily_download_quota(json_object *j);
int get_daily_download_wifi(json_object *j);
int64_t get_daily_download_cellular(json_object *j);
const char * get_sdk_capabilities(json_object *j);
int get_daily_download_manifest(json_object *j);
int get_max_content_duration(json_object *j);
const char * get_play_ads(json_object *j);
const char * get_skip_policy(json_object *j);
const char * get_tod_policy(json_object *j);
int64_t get_token_expiration(json_object *j);
const char * get_server_state(json_object *j);
const char * get_local_file(json_object *j);
const char * get_local_thumbfile(json_object *j);
char * get_local_nfo(json_object *j);
const char * get_local_file_size(json_object *j);
char * get_local_thumb_file(json_object *j);
int get_thumb_size(json_object *j);
time_t get_download_time();
const char * get_category(json_object *j);
const char * get_content_provider(json_object *j);
const char * get_unique_id(json_object *j);
const char * get_summary(json_object *j);
const char * get_title(json_object *j);
const char * get_time_stamp(json_object *j);
const char * get_duration(json_object *j);
const char * get_sdk_metadata(json_object *j);
const char * get_streams(json_object *j);
const char * get_adserver_url(json_object *j);
const char * get_tags(json_object *j);
const char * get_priority(json_object *j);
const char * get_object_type(json_object *j);
const char * get_thumb_attribs(json_object *j);
const char * get_object_attribs(json_object *j);
const char * get_children(json_object *j);
const char * get_policy_name(json_object *j);
const char * get_key_server_url(json_object *j);
char * quotesql(const char * s);
char * quotesqlint(char * s);
char * parse_provider_list(char * jstr);
bool quotsqlbool(bool b);


#endif
