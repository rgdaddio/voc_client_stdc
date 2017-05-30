#include "client.h"
#include "client_json.h"
#include "client_manifest.h"
#include "client_registration.h"
#include <json/json.h>
#include <json/json_object.h>
#include <stdio.h>
#include <string.h>

int register_user(char * arv1, char * arv2, char * arv3, char * arv4)
{

  char * path = "/Anaina/v0/Register";
  eXtype type = eRegister;
  char * jstr = get_response(arv1, arv2, arv3, arv4, path, type);

  // do the registration validation..
  handle_registration_json(jstr, arv1);

  return 0;
}


int get_manifest_from_server(char * arv1, char * arv2, char * arv3, char * arv4)
{
  char * path = "/Anaina/v0/Download-Manifest";
  eXtype type = eGetManifest;

  char * jstr = get_response(arv1, arv2, arv3, arv4, path, type);

  printf("\n\njstr = %s\n\n",jstr);
  printf("\n\njstr len = %ld\n\n",strlen(jstr));
  if(strlen(jstr)){
    install_cache(jstr);
    system("test/generate_voc_html.py"); //Create output file Jon's python script
  }

  return 0;
}


int hello_voc(char * arv1, char * arv2, char * arv3, char * arv4)
{
  char * path = "/Anaina/v0/HelloVoC";
  eXtype type = eHello;

  char * jstr = get_response(arv1, arv2, arv3, arv4, path, type);

  printf("json = %s\n",jstr);

  return 0;
}


int get_provider_list(char * arv1, char * arv2, char * arv3, char * arv4){
  char * path = "/Anaina/v0/Provider-List";
  eXtype type = eProviderList;
  char * jstr = get_response(arv1, arv2, arv3, arv4, path, type);
  printf("json = %s\n",jstr);
  handle_provider_list(jstr, arv1);
  return 0;
}

int get_topic_list(char * arv1, char * arv2, char * arv3, char * arv4){
  char * path = "/Anaina/v0/topic_list";
  eXtype type = eTopicList;
  char * jstr = get_response(arv1, arv2, arv3, arv4, path, type);
  printf("json = %s\n",jstr);
  return 0;
}

int get_profile(char * arv1, char * arv2, char *  arv3, char *  arv4){
  char *  path = "/Anaina/v0/profile";
  eXtype type = eProfile;
  char * jstr = get_response(arv1, arv2, arv3, arv4, path, type);
  printf("json = %s\n",jstr);
  return 0;
}

int get_policy(char * arv1, char * arv2, char * arv3, char * arv4){
  char * path = "/Anaina/v0/getPolicy";
  eXtype type = ePolicy;
  char * jstr = get_response(arv1, arv2, arv3, arv4, path, type);
  printf("json = %s\n",jstr);
  return 0;
}


int send_status(char * arv1, char * arv2, char * arv3, char * arv4)
{
  char * path = "/Anaina/v0/Status";
  eXtype type = eStatus;
  char * jstr = get_response(arv1, arv2, arv3, arv4, path, type);
  printf("json = %s\n",jstr);
  return 0;
}

int main(int argc, char* argv[])
{
    if (argc != 5)
      {
	printf( "Usage: client <server> <schemaName> <tenantId> <publicKey>\n");
	return 1;
      }
    printf("Validating the user\n");
    char * user = validate_user_for_registration();
    printf("User - %s\n",user);

    if(strlen(user))
      register_user(argv[1], argv[2], argv[3], argv[4]);
    else
      printf("User is previously registered\n");


 while(1)
   {
     printf("\nOptions: <cacheFill> <hello> <getProviderList> <getTopicList> <getProfile> <getPolicy> <status> <^C> ");
     char val[128];
     fscanf(stdin,"%s",val);

     if(strcmp(val,"cacheFill") == 0){
       get_manifest_from_server(argv[1], argv[2], argv[3], argv[4]);
     }
     else if(strcmp(val,"hello") == 0){
        hello_voc(argv[1], argv[2], argv[3], argv[4]);
     }
     else if(strcmp(val,"getProviderList") == 0){
        get_provider_list(argv[1], argv[2], argv[3], argv[4]);
     }
     else if(strcmp(val,"getTopicList") == 0){
        get_topic_list(argv[1], argv[2], argv[3], argv[4]);
     }
     else if(strcmp(val,"getProfile") == 0){
        get_profile(argv[1], argv[2], argv[3], argv[4]);
     }
     else if(strcmp(val,"getPolicy") == 0){
        get_policy(argv[1], argv[2], argv[3], argv[4]);
     }
     else if(strcmp(val,"status") == 0){
        send_status(argv[1], argv[2], argv[3], argv[4]);
     }
    else{
      printf("Unknown Option: %s\n",val);
    }
   }
 return 0;
}
