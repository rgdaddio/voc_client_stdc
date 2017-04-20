#include "client.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "json/json.h"
#include "json/json_object.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
void parse_url(const char * url, char * domain, char * path, char * proto)
{

  char * ret = strstr(url,"://");
  if(ret) {
    strncpy(proto,url,ret-url);
    proto[ret-url+1] = '\0';
    ret += 3;
    char * ret2 = strchr(ret,'/');
    if(ret2) {
      strncpy(domain,ret,ret2-ret);
      domain[ret2-ret+1] = '\0';
      ret = strchr(ret2,'?');
      if(ret) {
	strncpy(path,ret2,ret-ret2);
	path[ret-ret2+1] = '\0';
      }
      else {
	strcpy(path,ret2);
      }

    }

  }

  printf("proto = \"%s\"\n", proto);
  printf("domain = \"%s\"\n", domain);
  printf("path = \"%s\"\n", path);

}


int https_downloader(char * domain, char * path, int file_type, char * suffix)
{
  char cache_file[1024];
  sprintf(cache_file,"%s%s","test/cache/",suffix);
  int ret = get_content(domain,path,cache_file);

  return ret;
}
int http_downloader(char * domain, char * path, int file_type, char * suffix)
{
  char cache_file[1024];
  sprintf(cache_file,"%s%s","test/cache/",suffix);
  int ret = get_http_content(domain,path,cache_file);

  return ret;
}


int downloader(json_object *str, int file_type, char * suffix)
{
  char *  dir = "test/cache/";
  char path[1024] = {0};
  char domain[1024] = {0};
  char proto[16] = {0};
  parse_url(json_object_get_string(str), domain, path, proto);


  if (0 != access(dir, F_OK)) {
    if (ENOENT == errno) {
      printf("Directory %s does not exist. Creating it\n",dir);
      mkdir(dir, 0755);
    }
    else if (ENOTDIR == errno) {
      printf("%s not a Directory\n",dir);
      return -1;
    }
  }
  if(!strcmp(proto,"https")) {
    // use thread later..
    https_downloader(domain,path,file_type,suffix);
  }
  else if(!strcmp(proto,"http")) {
    // use thread later..
    http_downloader(domain,path,file_type,suffix);
  }
  else {
    printf("Unknown protocol %s\n",proto);
    return -1;
  }
  return 0;
}


int clear_cache()
{
  system("rm -rf test/cache/");
  return 0;
}
