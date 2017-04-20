#ifndef _MANIFEST_H_
#define _MANIFEST_

#include <string.h>

#define MAX_SIZE 1000

typedef struct key_val_t {
 char * key;
 char * value;
} key_val_s;

typedef struct key_val_db_t {
  int count;
  struct key_val_t arr[MAX_SIZE];
} key_val_db_s;

extern char * get_val(key_val_db_s * db, char * key);
extern void manifest_processing(key_val_db_s *db);
extern int install_cache(char * json);
//extern int downloader(json_object *str, int file_type);

#endif
