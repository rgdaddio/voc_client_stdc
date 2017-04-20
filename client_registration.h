#ifndef _REGISTRATION_H_
#define _REGISTRATION_H_
int handle_registration_json(char * jstr, char * server);
int handle_provider_list(char * jstr, char * server);
char * validate_user_for_registration(void);
#endif
