#ifndef _CLIENT__H_
#define _CLIENT__H_
typedef enum{
  eRegister = 1,
  eGetManifest,
  eStatus,
  eHello,
  eProviderList,
  eTopicList,
  eProfile,
  ePolicy
}eXtype;

char * get_response(char * server, char * schemaName, char * tenantId, char * publicKey, char * path, eXtype type);
int get_content(char * domain, char * path, char * cache_file);
int get_http_content(char * domain, char * path, char * cache_file);




#endif
