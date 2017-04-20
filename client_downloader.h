#ifndef __HTTPS_DOWNLOADER_H_
#define __HTTPS_DOWNLOADER_H_
int https_downloader(char * domain, char * path, int file_type);
int http_downloader(char * domain, char * path, int file_type);
int downloader(json_object *str, int file_type, char * suffix);
int clear_cache();
#endif
