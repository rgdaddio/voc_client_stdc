#include<stdio.h>
#include<string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <strings.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "client_manifest.h"
#define FAIL    -1

#ifndef OPENSSL_NO_SSL2
# define OPENSSL_NO_SSL2
#endif

/* 
 * error - wrapper for perror
 */
void error(char *msg) {
  perror(msg);
  exit(0);
}

#include "client.h"
char * get_reg_json(char * schema,char * tenant, char * pubkey)
{
  static char oss[1024];
  sprintf(oss,"%s%s%s%s%s%s%s",
	  "{\"serverState\":{\"schemaName\":\"",
	  schema,
	  "\",\"tenantId\":\"",
	  tenant,
	  "\"},\"publicKey\":\"",
	  pubkey,
	  "\",\"platform\"" ":" "\"linux\",\"deviceId\"" ":\"623bce38-a1f4-11e6-bb6c-3417eb9985a6\",\"deviceType\":\"pc\",\"pushToken\":\"tt\",\"version\":\"17.2.3\"""}");    
  return oss;
}


char * build_http_post_header(char * server, char * json,char * path)
{
  static char oss[1024];
  sprintf(oss,
	  "%s%s%s%s%s%zu%s%s",
	  "POST ",
	  path,
	  " HTTP/1.1 \r\nHost:",
	  server,
	  "\r\nUser-Agent: C/1.0\r\nContent-Type: application/json\r\nAccept: */*\r\nContent-Length: ",
	  strlen(json),
	  "\r\nConnection: close\r\n\r\n", /*NOTE THE Double line feed*/
	  json
	  );  
  return oss;
}

#if 0
void client::build_http_get_header(std::string server_ip, std::string json)
{
  std::ostream request_stream(&request_);

  request_stream << "GET "  << cpath << " HTTP/1.1 \r\n";
  request_stream << "Host:"  << servr << "\r\n";
  request_stream << "Accept: */*\r\n";
  request_stream << "Connection: close\r\n\r\n";  //NOTE THE Double line feed                                                              
}
#endif

char * get_status_json(char * schema, char * tenant)
{
  //TODO will need to access the database and get status related variables to put in here 
  static key_val_db_s json;
  json.count =0;
  
  manifest_processing(&json);
   
  static char oss[1024];
  sprintf(oss,"%s%s%s%s%s%s%s%s%s%s%s",
	  "{\"serverState\":{\"schemaName\":\"",
	  schema,
	  "\",\"tenantId\"" ":\"",
	  tenant, 
	  "\"},\"vocId\":\"", 
	  get_val(&json, "voc_id"),
	  "\",\"platform\"" ":" "\"linux\",\"deviceId\"" ":\"623bce38-a1f4-11e6-bb6c-3417eb9985a6\",\"deviceType\":\"pc\",\"refreshToken\":\"", 
	  get_val(&json, "refresh_token"), 
	  "\",\"accessToken\":\"", 
	  get_val(&json, "access_token"), 
	  "\",\"version\":\"17.2.3\", \"deviceStatus\":{\"charger\":true}}");    
  return oss;
}


 char * get_req_json(char * schema, char * tenant)
 {

   static key_val_db_s json;
   json.count =0;
   
   manifest_processing(&json);

  
  static char oss[1024];
  sprintf(oss,"%s%s%s%s%s%s%s%s%s%s%s",
	  "{\"serverState\":{\"schemaName\":\"",
	  schema,
	  "\",\"tenantId\"" ":\"",
	  tenant, 
	  "\"},\"vocId\":\"", 
	  get_val(&json, "voc_id"),
	  "\",\"platform\"" ":" "\"linux\",\"deviceId\"" ":\"623bce38-a1f4-11e6-bb6c-3417eb9985a6\",\"deviceType\":\"pc\",\"refreshToken\":\"", 
	  get_val(&json, "refresh_token"), 
	  "\",\"accessToken\":\"", 
	  get_val(&json, "access_token"), 
	  "\",\"version\":\"17.2.3\"}");    
  return oss;

  #if 0
  std::map<std::string, std::string> json = manifest_processing();
  std::ostringstream oss;
  oss << "{" << "\"serverState\"" << ":" << "{" << "\"schemaName\"" << ":" << "\"" << schma << "\"" << "," << "\"tenantId\"" ":" 
      << "\""<< tenant << "\"" << "}" << "," << "\"vocId\"" << ":" << "\"" << json["voc_id"]  << "\"" << "," << "\"platform\"" ":" "\"linux\"" 
      << "," << "\"deviceId\"" ":" << "\"623bce38-a1f4-11e6-bb6c-3417eb9985a6\"" << "," << "\"deviceType\"" << ":" << "\"pc\"" 
      << "," << "\"refreshToken\"" << ":" << "\"" << json["refresh_token"] << "\"" << "," << "\"accessToken\"" << ":" << "\"" << json["access_token"] << "\"" << "," << "\"version\"" << ":" << "\"17.2.3\"""}";    
  return oss.str();
  #endif 
}


/*---------------------------------------------------------------------*/
/*--- OpenConnection - create socket and connect to server.         ---*/
/*---------------------------------------------------------------------*/
int OpenConnection(const char *hostname, int port)
{   
  int sd;
  struct hostent *host;
  struct sockaddr_in addr;

  if ( (host = gethostbyname(hostname)) == NULL )
    {
      perror(hostname);
      abort();
    }
  sd = socket(PF_INET, SOCK_STREAM, 0);
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = *(long*)(host->h_addr_list[0]);
  if ( connect(sd, (const struct sockaddr *)&addr, sizeof(addr)) != 0 )
    {
      close(sd);
      perror(hostname);
      abort();
    }
  return sd;
}

/*---------------------------------------------------------------------*/
/*--- InitCTX - initialize the SSL engine.                          ---*/
/*---------------------------------------------------------------------*/
SSL_CTX* InitCTX(void)
{   
  const SSL_METHOD *method;
  SSL_CTX *ctx;

  OpenSSL_add_all_algorithms();/* Load cryptos, et.al. */
  SSL_load_error_strings();/* Bring in and register error messages */
  method = SSLv3_client_method();/* Create new client-method instance */
  ctx = SSL_CTX_new(method);/* Create new context */
  if ( ctx == NULL )
    {
      ERR_print_errors_fp(stderr);
      abort();
    }
  return ctx;
}

/*---------------------------------------------------------------------*/
/*--- ShowCerts - print out the certificates.                       ---*/
/*---------------------------------------------------------------------*/
void ShowCerts(SSL* ssl)
{   X509 *cert;
  char *line;

  cert = SSL_get_peer_certificate(ssl);/* get the server's certificate */
  if ( cert != NULL )
    {
      //printf("Server certificates:\n");
      line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
      //printf("Subject: %s\n", line);
      free(line);/* free the malloc'ed string */
      line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
      //printf("Issuer: %s\n", line);
      free(line);/* free the malloc'ed string */
      X509_free(cert);/* free the malloc'ed certificate copy */
    }
  else
    printf("No certificates.\n");
}


#if 0
char * raise_request(char * hostname, int portnum, char * buf, char * resp, int resp_len)
{
  SSL_CTX *ctx;
  int server;
  SSL *ssl;
  int bytes;


  ctx = InitCTX();
  server = OpenConnection(hostname, portnum);
  ssl = SSL_new(ctx);/* create new SSL connection state */
  SSL_set_fd(ssl, server);/* attach the socket descriptor */
  if ( SSL_connect(ssl) == FAIL )/* perform the connection */
    ERR_print_errors_fp(stderr);
  else
    {   
      //printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
      ShowCerts(ssl);/* get any certs */
      SSL_write(ssl, buf, strlen(buf));/* encrypt & send message */
      bytes = SSL_read(ssl, resp, resp_len);/* get reply & decrypt */
      buf[bytes] = 0;
      //printf("Received: \"%s\"\n", buf);
      SSL_free(ssl);/* release connection state */
    }
  close(server);/* close socket */
  SSL_CTX_free(ctx);/* release context */
}
char * raise_request(char * hostname, int portno, char * buf, char * resp, int resp_len)
{
  BIO               *outbio = NULL;
  X509                *cert = NULL;
  X509_NAME       *certname = NULL;
  const SSL_METHOD *method;
  SSL_CTX *ctx;
  SSL *ssl;


  int sockfd, n;
  struct sockaddr_in serveraddr;
  struct hostent *server;


  /* ---------------------------------------------------------- *
   * These function calls initialize openssl for correct work.  *
   * ---------------------------------------------------------- */
  OpenSSL_add_all_algorithms();
  ERR_load_BIO_strings();
  ERR_load_crypto_strings();
  SSL_load_error_strings();

  /* ---------------------------------------------------------- *
   * Create the Input/Output BIO's.                             *
   * ---------------------------------------------------------- */
  outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);

  /* ---------------------------------------------------------- *
   * initialize SSL library and register algorithms             *
   * ---------------------------------------------------------- */
  if(SSL_library_init() < 0)
    BIO_printf(outbio, "Could not initialize the OpenSSL library !\n");

  /* ---------------------------------------------------------- *
   * Set SSLv2 client hello, also announce SSLv3 and TLSv1      *
   * ---------------------------------------------------------- */
  method = SSLv23_client_method();

  /* ---------------------------------------------------------- *
   * Try to create a new SSL context                            *
   * ---------------------------------------------------------- */
  if ( (ctx = SSL_CTX_new(method)) == NULL)
    BIO_printf(outbio, "Unable to create a new SSL context structure.\n");

  /* ---------------------------------------------------------- *
   * Disabling SSLv2 will leave v3 and TSLv1 for negotiation    *
   * ---------------------------------------------------------- */
  SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);

  /* ---------------------------------------------------------- *
   * Create new SSL connection state object                     *
   * ---------------------------------------------------------- */
  ssl = SSL_new(ctx);


  /* socket: create the socket */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) 
    error("ERROR opening socket");


  /* gethostbyname: get the server's DNS entry */
  server = gethostbyname(hostname);
  if (server == NULL) {
    fprintf(stderr,"ERROR, no such host as %s\n", hostname);
    exit(0);
  }

  /* build the server's Internet address */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  bcopy((char *)server->h_addr_list[0], 
	(char *)&serveraddr.sin_addr.s_addr, server->h_length);
  serveraddr.sin_port = htons(portno);

  /* connect: create a connection with the server */
  if (connect(sockfd, (const struct sockaddr *)&serveraddr, (socklen_t)sizeof(serveraddr)) < 0) 
    error("ERROR connecting");

  SSL_set_fd(ssl, sockfd);

  /* ---------------------------------------------------------- *
   * Try to SSL-connect here, returns 1 for success             *
   * ---------------------------------------------------------- */
  if ( SSL_connect(ssl) != 1 )
    BIO_printf(outbio, "Error: Could not build a SSL session to: %s.\n", hostname);
  else
    BIO_printf(outbio, "Successfully enabled SSL/TLS session to: %s.\n", hostname);

  /* ---------------------------------------------------------- *
   * Get the remote certificate into the X509 structure         *
   * ---------------------------------------------------------- */
  cert = SSL_get_peer_certificate(ssl);
  if (cert == NULL)
    BIO_printf(outbio, "Error: Could not get a certificate from: %s.\n", hostname);
  else
    BIO_printf(outbio, "Retrieved the server's certificate from: %s.\n", hostname);

  /* ---------------------------------------------------------- *
   * extract various certificate information                    *
   * -----------------------------------------------------------*/
  certname = X509_NAME_new();
  certname = X509_get_subject_name(cert);

  /* ---------------------------------------------------------- *
   * display the cert subject here                              *
   * -----------------------------------------------------------*/
  BIO_printf(outbio, "Displaying the certificate subject data:\n");
  X509_NAME_print_ex(outbio, certname, 0, 0);
  BIO_printf(outbio, "\n");


  /* send the message line to the server */
  n = BIO_write(ssl, buf, strlen(buf));
  if (n < 0) 
    error("ERROR writing to socket");

  n = read(sockfd, resp, resp_len);
  if (n < 0) 
    error("ERROR reading from socket");
  //printf("Response from server: %s", resp);
  SSL_free(ssl);
  close(sockfd);
  X509_free(cert);
  SSL_CTX_free(ctx);
  return 0;
}
#endif

/**
 * Simple log function
 */
void slog(char* message) {
  fprintf(stdout, "%s\n",message);
}

/**
 * Print SSL error details
 */
void print_ssl_error(char* message, FILE* out) {

  fprintf(out, "%s\n",message);
  fprintf(out, "Error: %s\n", ERR_reason_error_string(ERR_get_error()));
  fprintf(out, "%s\n", ERR_error_string(ERR_get_error(), NULL));
  ERR_print_errors_fp(out);
}

/**
 * Print SSL error details with inserted content
 */
void print_ssl_error_2(char* message, char* content, FILE* out) {

  fprintf(out, message, content);
  fprintf(out, "Error: %s\n", ERR_reason_error_string(ERR_get_error()));
  fprintf(out, "%s\n", ERR_error_string(ERR_get_error(), NULL));
  ERR_print_errors_fp(out);
}

/**
 * Initialise OpenSSL
 */
void init_openssl() {

  /* call the standard SSL init functions */
  SSL_load_error_strings();
  SSL_library_init();
  ERR_load_BIO_strings();
  OpenSSL_add_all_algorithms();

  /* seed the random number system - only really nessecary for systems without '/dev/random' */
  /* RAND_add(?,?,?); need to work out a cryptographically significant way of generating the seed */
}


/**
 * Connect to a host using an encrypted stream
 */
BIO* connect_encrypted(char* host_and_port, char* store_path, SSL_CTX** ctx, SSL** ssl) {

  BIO* bio = NULL;
  /* Set up the SSL pointers */
  *ctx = SSL_CTX_new(TLSv1_client_method());
  *ssl = NULL;

  #if 0
  r = SSL_CTX_load_verify_locations(*ctx, store_path, NULL);

  if (r == 0) {

    print_ssl_error_2("Unable to load the trust store from %s.\n", store_path, stdout);
    return NULL;
  }
  #endif 

  /* Setting up the BIO SSL object */
  bio = BIO_new_ssl_connect(*ctx);
  BIO_get_ssl(bio, ssl);
  if (!(*ssl)) {

    print_ssl_error("Unable to allocate SSL pointer.\n", stdout);
    return NULL;
  }
  SSL_set_mode(*ssl, SSL_MODE_AUTO_RETRY);

  /* Attempt to connect */
  BIO_set_conn_hostname(bio, host_and_port);

  /* Verify the connection opened and perform the handshake */
  if (BIO_do_connect(bio) < 1) {

    print_ssl_error_2("Unable to connect BIO.%s\n", host_and_port, stdout);
    return NULL;
  }

  if (SSL_get_verify_result(*ssl) != X509_V_OK) {

    print_ssl_error("Unable to verify connection result.\n", stdout);
  }

  return bio;
}

/**
 * Read a from a stream and handle restarts if nessecary
 */
ssize_t read_from_stream(BIO* bio, char* buffer, ssize_t length, char * cache_file) 
{
  ssize_t r = -1;
  int a = 0;
  FILE * cfd = NULL;
  if(cache_file){
    cfd = fopen(cache_file,"w");
  }
  int found_header = 0;  
  for(;;) {
    r = BIO_read(bio, buffer+a, length-a-1);
    a += r;
    //printf("\n\nr= %d\n\n",(int)r);
    if (r == 0) {
      print_ssl_error("Reached the end of the data stream.\n", stdout);
      break;
    } 
    else if (r < 0) {
      if (!BIO_should_retry(bio)) {
	print_ssl_error("BIO_read should retry test failed.\n", stdout);
	continue;
      }
      break;
      
      /* It would be prudent to check the reason for the retry and handle
       * it appropriately here */
    }
    if (cfd) {
      if(!found_header) {
	//strip the header
	char *ret;
	ret = strstr(buffer, "\r\n\r\n");
	if(ret) {
	  ret +=4;
	  r -= ret - buffer;
	  buffer = ret;
	  found_header = 1;
	}

      }
      a = 0;
      fwrite(buffer,r,1,cfd);
    }
    else if(a >= length-1) {
      // we should never reach here 
      printf("Cannot read fully...\n");
      return a;
    }
  
  }
  if(cfd) {
    fclose(cfd);
  }
  return r;
}

/**
 * Write to a stream and handle restarts if nessecary
 */
int write_to_stream(BIO* bio, char* buffer, ssize_t length) {

  ssize_t r = -1;

  while (r < 0) {

    r = BIO_write(bio, buffer, length);
    if (r <= 0) {

      if (!BIO_should_retry(bio)) {

	print_ssl_error("BIO_read should retry test failed.\n", stdout);
	continue;
      }

      /* It would be prudent to check the reason for the retry and handle
       * it appropriately here */
    }

  }

  return r;
}

int raise_http_request(char * hostname, int port, char * buf, char * resp, int length, char * cache_file)
{
  int sd;
  struct hostent *host;
  struct sockaddr_in addr;
  FILE * cfd = NULL;
  if(cache_file){
    cfd = fopen(cache_file,"w");
  }
  if ( (host = gethostbyname(hostname)) == NULL )
    {
      perror(hostname);
      abort();
    }
  sd = socket(PF_INET, SOCK_STREAM, 0);
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = *(long*)(host->h_addr_list[0]);

  if ( connect(sd, (const struct sockaddr *)&addr, sizeof(addr)) != 0 )
    {
      close(sd);
      printf("connect failed aborting\n");
      perror(hostname);
      abort();
    }

  // sending the request
  if(send(sd,buf,strlen(buf),0)<0) {
      perror("send failed.. aborting");
      close(sd);
      abort();
  }
  int a = 0;
  int found_header = 0;
  for(;;) {
    int ret = recv(sd,resp,length-1,0);
    printf("#");
    fflush(stdout);
    if(ret > 0) {
      a += ret;
      if(cfd) {
	if(!found_header) {
	  //strip the header
	  char *rn;
	  rn = strstr(resp, "\r\n\r\n");
	  if(rn) {
	    rn +=4;
	    ret -= rn - resp;
	    resp = rn;
	    found_header = 1;
	  }
	}

	fwrite(resp,ret,1,cfd);
      }
    }
    else if(ret == 0) {
      //no more data
      if(cfd) fclose(cfd);
      close(sd);
      printf("\n");
      return a;
    }
    else {
      printf("error reading content\n");
      if(cfd) fclose(cfd);
      close(sd);
      printf("\n");
      return -1;
    }

  }
}


int raise_request(char * hostname, int portno, char * buf, char * resp, int resp_len, char * cache_file)
{
  BIO* bio;

  BIO               *outbio = NULL;
  X509                *cert = NULL;
  X509_NAME       *certname = NULL;
  const SSL_METHOD *method;
  SSL_CTX *ctx;
  SSL *ssl;


  int sockfd;
  struct sockaddr_in serveraddr;
  struct hostent *server;


  /* ---------------------------------------------------------- *
   * These function calls initialize openssl for correct work.  *
   * ---------------------------------------------------------- */
  OpenSSL_add_all_algorithms();
  ERR_load_BIO_strings();
  ERR_load_crypto_strings();
  SSL_load_error_strings();

  /* ---------------------------------------------------------- *
   * Create the Input/Output BIO's.                             *
   * ---------------------------------------------------------- */
  outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);

  /* ---------------------------------------------------------- *
   * initialize SSL library and register algorithms             *
   * ---------------------------------------------------------- */
  if(SSL_library_init() < 0)
    BIO_printf(outbio, "Could not initialize the OpenSSL library !\n");

  /* ---------------------------------------------------------- *
   * Set SSLv2 client hello, also announce SSLv3 and TLSv1      *
   * ---------------------------------------------------------- */
  method = SSLv23_client_method();

  /* ---------------------------------------------------------- *
   * Try to create a new SSL context                            *
   * ---------------------------------------------------------- */
  if ( (ctx = SSL_CTX_new(method)) == NULL)
    BIO_printf(outbio, "Unable to create a new SSL context structure.\n");

  /* ---------------------------------------------------------- *
   * Disabling SSLv2 will leave v3 and TSLv1 for negotiation    *
   * ---------------------------------------------------------- */
  SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);

  /* ---------------------------------------------------------- *
   * Create new SSL connection state object                     *
   * ---------------------------------------------------------- */
  ssl = SSL_new(ctx);

  /* socket: create the socket */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) 
    error("ERROR opening socket");


  /* gethostbyname: get the server's DNS entry */
  server = gethostbyname(hostname);
  if (server == NULL) {
    fprintf(stderr,"ERROR, no such host as %s\n", hostname);
    exit(0);
  }

  /* build the server's Internet address */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  bcopy((char *)server->h_addr_list[0], 
	(char *)&serveraddr.sin_addr.s_addr, server->h_length);
  serveraddr.sin_port = htons(portno);

  /* connect: create a connection with the server */
  if (connect(sockfd, (const struct sockaddr *)&serveraddr, (socklen_t)sizeof(serveraddr)) < 0) 
    error("ERROR connecting");

  SSL_set_fd(ssl, sockfd);

  /* ---------------------------------------------------------- *
   * Try to SSL-connect here, returns 1 for success             *
   * ---------------------------------------------------------- */
  if ( SSL_connect(ssl) != 1 )
    BIO_printf(outbio, "Error: Could not build a SSL session to: %s.\n", hostname);
  else
    BIO_printf(outbio, "Successfully enabled SSL/TLS session to: %s.\n", hostname);

  /* ---------------------------------------------------------- *
   * Get the remote certificate into the X509 structure         *
   * ---------------------------------------------------------- */
  cert = SSL_get_peer_certificate(ssl);
  if (cert == NULL)
    BIO_printf(outbio, "Error: Could not get a certificate from: %s.\n", hostname);
  else
    BIO_printf(outbio, "Retrieved the server's certificate from: %s.\n", hostname);

  /* ---------------------------------------------------------- *
   * extract various certificate information                    *
   * -----------------------------------------------------------*/
  certname = X509_NAME_new();
  certname = X509_get_subject_name(cert);

  /* ---------------------------------------------------------- *
   * display the cert subject here                              *
   * -----------------------------------------------------------*/
  BIO_printf(outbio, "Displaying the certificate subject data:\n");
  X509_NAME_print_ex(outbio, certname, 0, 0);
  BIO_printf(outbio, "\n");


  /* initilise the OpenSSL library */
  init_openssl();

  char host_and_port[1000];
  sprintf(host_and_port,"%s:%d",hostname,portno);

  if ((bio = connect_encrypted(host_and_port, (char * )"NULL", &ctx, &ssl)) == NULL)
    return (-1);


  write_to_stream(bio, buf, strlen(buf));
  read_from_stream(bio, resp, 64000,cache_file);

  /* clean up the SSL context resources for the encrypted link */
  SSL_CTX_free(ctx);



  #if 0
  /* send the message line to the server */
  n = write(sockfd, buf, strlen(buf));
  if (n < 0) 
    error("ERROR writing to socket");

  int b = read(sockfd, resp, resp_len);
  if (b < 0) 
    error("ERROR reading from socket");
  #endif 
  //printf("Response from server: %s", resp);

  SSL_free(ssl);
  close(sockfd);
  X509_free(cert);
  return 0;
}


int get_content(char * domain, char * path, char * cache_file)
{
  char request[1024];
  sprintf(request,
	  "%s%s%s%s%s",
	  "GET ",
	  path,
	  " HTTP/1.1 \r\nHost:",
	  domain,
	  "\r\nAccept: */*\r\nConnection: close\r\n\r\n" /*NOTE THE Double line feed*/
	  );  

  static char resp[64000];
  memset(&resp[0], 0, sizeof(resp));
  raise_request(domain, 443, request, resp, 64000,cache_file);
  return 0;
}

int get_http_content(char * domain, char * path, char * cache_file)
{
  char request[1024];
  sprintf(request,
	  "%s%s%s%s%s",
	  "GET ",
	  path,
	  " HTTP/1.1 \r\nHost:",
	  domain,
	  "\r\nAccept: */*\r\nConnection: close\r\n\r\n" /*NOTE THE Double line feed*/
	  );  

  static char resp[64000];
  memset(&resp[0], 0, sizeof(resp));
  raise_http_request(domain, 80, request, resp, 64000,cache_file);
  return 0;
}

char * get_response(char * server, char * schemaName, char * tenantId, char * publicKey, char * path, eXtype type)
{
  char * json = ""; 
  char * with_header = "";
  if(type == eRegister)
    json = get_reg_json(schemaName,tenantId,publicKey);
  else if(type == eGetManifest || type == eHello || type == eProviderList || type == eTopicList || type == eProfile || type == ePolicy)
    json = get_req_json(schemaName,tenantId);
  else if(type == eStatus)
    json = get_status_json(schemaName,tenantId);
  else
    printf("No further operation.. type %d not defined\n",type);
  if(type ==  eRegister || type == eGetManifest || type == eStatus || type ==eHello || type == eProviderList || type == eTopicList || type == eProfile || type == ePolicy)
    with_header = build_http_post_header(server, json,path);
  else
    printf("Unknown operation type\n");
  
  //printf("\n\nRequest: %s\n",with_header);
  static char resp[64000];
  memset(&resp[0], 0, sizeof(resp));
  raise_request(server, 443, with_header, resp, 64000,NULL);


  //strip the header
  char *ret;
  ret = strstr(resp, "\r\n\r\n");

  if(ret)
    {
      ret += 4;
    }

  return ret;
}

