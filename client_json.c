//#include "client.h"
#include "stdio.h"
#include <sqlite3.h>
#include "json/json.h"
#include "json/json_object.h"
#include <openssl/sha.h>
#include <string.h>
#include <time.h>

#include "client_json.h"
#include "client_manifest.h"
#include "client_db.h"

#include "client_downloader.h"

//
//
//json-c interface file kept in 'c' style for portability
//
//
void get_sha1(const char * str, int len, char * hash)
{
  static char temp[SHA_DIGEST_LENGTH];

  memset(hash, 0x0, SHA_DIGEST_LENGTH*2);
  memset(temp, 0x0, SHA_DIGEST_LENGTH);

  SHA1((const unsigned char *) str, len, (unsigned char *)temp);
  for (int i=0; i < SHA_DIGEST_LENGTH; i++) {
    sprintf((char*)&(hash[i*2]), "%02x", temp[i]);
  }
}

const char * get_voc_id(json_object *j)
{
  //std::cout << "vocid" << std::endl;
  json_object *tmp;
  json_object_object_get_ex(j, "vocId", &tmp);

  return json_object_get_string(tmp);
}

char * get_platform(json_object *j)
{
  return "Linux";
}

char * get_device_id(json_object *j)
{
  return "000000DEADBEEF";
}

char * get_device_type(json_object *j)
{
  return "000000DEADBEEF";
}

const char * get_access_token(json_object *j)
{
  json_object *tmp;
  json_object_object_get_ex(j, "accessToken", &tmp);
  return json_object_get_string(tmp);
}


const char * get_refresh_token(json_object *j)
{
  json_object *tmp;
  json_object_object_get_ex(j, "refreshToken", &tmp);
  return json_object_get_string(tmp);
}

const char * get_congestion_detection(json_object *j)
{
  json_object *tmp;
  json_object_object_get_ex(j, "congestionDetection", &tmp);
  if(!tmp)
    return "";
  return json_object_get_string(tmp);
}

const char * get_ads_frequency(json_object *j)
{
  json_object *tmp;
  json_object_object_get_ex(j, "adsFrequency", &tmp);
  return json_object_get_string(tmp);
}

const char * get_daily_download_quota(json_object *j)
{
  json_object *tmp;
  json_object_object_get_ex(j, "dailyDownloadQuota", &tmp);
  if(!tmp)
    return "";
  return json_object_get_string(tmp);
}

int get_daily_download_wifi(json_object *j)
{
  json_object *tmp;
  json_object_object_get_ex(j, "dailyDownloadWifi", &tmp);
  if(!tmp)
    return 0;
  return json_object_get_int(tmp);
}

int64_t get_daily_download_cellular(json_object *j)
{
  json_object *tmp;
  json_object_object_get_ex(j, "dailyDownloadCellular", &tmp);
  if(!tmp){
    printf("FAIL on get !!!!!!!!!!!!\n");
    return 0;
  }
  return json_object_get_int64(tmp);
}

const char *  get_sdk_capabilities(json_object *j)
{
  json_object *tmp;
  json_object_object_get_ex(j, "sdkCapabilities", &tmp);
  return json_object_get_string(tmp);
}

int get_daily_download_manifest(json_object *j)
{
  json_object *tmp;
  json_object_object_get_ex(j, "dailyManifestCount", &tmp);
  if(!tmp)
    return 0;
  return json_object_get_int(tmp);
}

int get_max_content_duration(json_object *j)
{
  json_object *tmp;
  json_object_object_get_ex(j, "maxContentDuration", &tmp);
  if(!tmp)
    return 0;
  return json_object_get_int(tmp);
}

const char * get_play_ads(json_object *j)
{
  json_object *tmp;
  json_object_object_get_ex(j, "playAds", &tmp);
  return json_object_get_string(tmp);
}

const char * get_skip_policy(json_object *j)
{
  json_object *tmp;
  json_object_object_get_ex(j, "skipPolicyFirstTime", &tmp);
  return json_object_get_string(tmp);
}

const char * get_tod_policy(json_object *j)
{
  json_object *tmp;
  json_object_object_get_ex(j, "todPolicy", &tmp);
  return json_object_get_string(tmp);
}


int64_t get_token_expiration(json_object *j)
{
  json_object *tmp;
  json_object_object_get_ex(j, "tokenExpiryDate", &tmp);
  if(!tmp)
    return 0;
  return json_object_get_int64(tmp);
}

const char * get_server_state(json_object *j)
{
  json_object *tmp;
  json_object_object_get_ex(j, "serverState", &tmp);
  if(!tmp)
    return "test";
  return json_object_get_string(tmp);
}

const char * get_local_file(json_object *j) //j is an array object
{
  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      json_object *lfile;
      bool status;
      if(!(status = json_object_object_get_ex(lobj, "streams", &objtor))) //TBD check status
	printf("Unable to retrieve stream from db\n");

      if(!(status = json_object_object_get_ex(lobj, "uniqueId", &lfile))) //TBD check status
	printf("Unable to retrieve uniqueId from db\n");

      for(int i = 0; i < json_object_array_length(objtor); i++)
	{
	  if(i == 1)//just grab the first one for now FIXME
	    break;
	  json_object *pobj = json_object_array_get_idx(objtor, i);



	  static char local_file[2*SHA_DIGEST_LENGTH+1];
	  get_sha1(json_object_get_string(lfile),strlen(json_object_get_string(lfile)),local_file);


	  json_object *str;
	  json_object_object_get_ex(pobj,"url", &str);

	  //mallocing free when needed
	  char * lf_copy = malloc(strlen(local_file)+8);
	  sprintf(lf_copy,"%s.1",local_file);



	  //change
	  downloader(str, 1, lf_copy); //need this but not now
	  //printf("returning %s\n",lf_copy);
	  return lf_copy;
	}
    }
  return "error";
}

const char * get_local_file_size(json_object *j)
{
  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      json_object *lfile;
      bool status = json_object_object_get_ex(lobj, "streams", &objtor); //TBD check status

      status = json_object_object_get_ex(lobj, "uniqueId", &lfile); //TBD check status

      for(int i = 0; i < json_object_array_length(objtor); i++)
        {
	  json_object *pobj = json_object_array_get_idx(objtor, i);
	  //char local_file[SHA_DIGEST_LENGTH*2];
	  //get_sha1(json_object_get_string(lfile),strlen(json_object_get_string(lfile)),local_file);


          json_object *str;
	  if((status = json_object_object_get_ex(pobj,"size", &str)))
	    {
	      //printf("Gonna return %s", json_object_get_string(str));
	      return json_object_get_string(str);
	    }
	}
    }
  return "error";
}

char * get_local_thumb_file(json_object *j) //j is an array object
{
  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      json_object *lfile;
      bool status;
      if((status = json_object_object_get_ex(lobj, "thumbFile", &objtor)))
	{
	  status = json_object_object_get_ex(lobj, "uniqueId", &lfile);
	 const  char * local_thumb = json_object_get_string(lfile);

	  //mallocing free when needed
	  char * lt_copy = malloc(strlen(local_thumb)+5);
	  sprintf(lt_copy,"%s.2",local_thumb);
	  //change it
	  downloader(objtor, 2, lt_copy);
	  return lt_copy;
	}
    }
  return "error";
}

char * get_local_nfo(json_object *j)
{
  return "not done";
}

int get_thumb_size(json_object *j)
{
  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      bool status;
      if((status = json_object_object_get_ex(lobj, "thumbSize", &objtor)))
	{
	  return json_object_get_int(objtor);
	}
    }
  return -1;
}

#if 0
boost::posix_time::time_duration::tick_type milliseconds_since_epoch()
{
  using boost::gregorian::date;
  using boost::posix_time::ptime;
  using boost::posix_time::microsec_clock;

  static ptime const epoch(date(1970, 1, 1));
  return (microsec_clock::universal_time() - epoch).total_milliseconds();
}
#endif

time_t seconds_since_epoch()
{
  time_t epoch = 0;
  gmtime(&epoch);
  return epoch;
}

time_t get_download_time()
{
  return seconds_since_epoch();
}

const char * get_content_provider(json_object *j) //j is an array object
{
  char * err = "error";
  char * empty = "null";
  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      bool status;
      if((status = json_object_object_get_ex(lobj, "provider", &objtor)))
        {
	  if(json_object_get_type(objtor) == json_type_null)
            return empty;
	  return json_object_get_string(objtor);
        }
    }
  return err;
}

const char * get_category(json_object *j) //j is an array object
{

  char * err = "error";

  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      bool status;
      if((status = json_object_object_get_ex(lobj, "catId", &objtor)))
        {
	  json_object *larr = json_object_array_get_idx(objtor, 0);
	  return json_object_get_string(larr);
	}
    }
  return err;
}

const char * get_unique_id(json_object *j) //j is an array object
{
  char * err = "error";
  char * empty = "null";
  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      bool status;
      if((status = json_object_object_get_ex(lobj, "uniqueId", &objtor)))
        {
	  if(json_object_get_type(objtor) == json_type_null)
            return empty;
          return json_object_get_string(objtor);
        }
    }
  return err;
}
char * quotesql(const char * actual)
{
  static char sum[4096];
  char * quote = strchr(actual, '\'');
  int len = 0;
  while(quote)
    {
      *quote = '\0';
      len += sprintf(sum+len,"%s''",actual);
      actual = quote+1;
      quote = strchr(actual, '\'');
    }
  sprintf(sum+len,"%s",actual);

  return sum;
}
const char * get_summary(json_object *j) //j is an array object
{
  char * err = "error";
  char * empty = "null";
  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      bool status;
      if((status = json_object_object_get_ex(lobj, "summary", &objtor)))
        {
	  if(json_object_get_type(objtor) == json_type_null)
            return empty;
	  return json_object_get_string(objtor);

        }
    }
  return err;
}

const char * get_title(json_object *j) //j is an array object
{
  char * err = "error";
  char * empty = "null";
  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      bool status;
      if((status = json_object_object_get_ex(lobj, "title", &objtor)))
	{
	  if(json_object_get_type(objtor) == json_type_null)
            return empty;
          return json_object_get_string(objtor);
	}
    }
  return err;
}

const char * get_time_stamp(json_object *j) //j is an array object
{

  char * err = "error";

  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      bool status;
      if((status = json_object_object_get_ex(lobj, "timeStamp", &objtor)))
	{
          return json_object_get_string(objtor);
	}
    }
  return err;
}

#if 0
static char * parse_json_comma_list(char * list, int item)
{
  char * err = "error";

  size_t pos = 0;
  char * delimiter = "=";
  char * dpos = strchr(list,'=');
  while(dpos)
    {

    }
  while ((pos = list.find(delimiter)) != std::string::npos) {
    token = list.substr(0, pos);
    if(item_count == item){
      std::string sub_delimiter = ",";
      size_t sub_pos = 0;
      while((sub_pos = token.find(sub_delimiter)) != std::string::npos){
	token.erase(sub_pos, sub_delimiter.length() + (token.length() - sub_pos));
	return token;
      }
    }
    item_count++;
    list.erase(0, pos + delimiter.length());
  }
  return err;
}
#endif


const char * get_duration(json_object *j)
{
  char * err = "10";

  #if 0
  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      bool status;
      if((status = json_object_object_get_ex(lobj, "objectAttrs", &objtor)))
        {
	  const char * attr = json_object_get_string(objtor);
	  attr.erase(std::remove(attr.begin(), attr.end(), ','), attr.end());
	  //std::cout << "attr " << attr << std::endl;
          //return parse_json_comma_list(json_object_get_string(objtor), 1); //duration 1st item
	  std::string dnld = parse_json_comma_list(json_object_get_string(objtor), 1);
	  if((dnld.empty()) || (dnld.compare("null")) || dnld.compare("empty"))
	    return err;
	  else
	    return dnld;
        }
    }
  #endif
  return err;
}


const char * get_sdk_metadata(json_object *j) //j is an array object
{
  char * err = "error";
  char * empty = "null";
  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      bool status;
      if((status = json_object_object_get_ex(lobj, "sdkMetadataPassthrough", &objtor)))
        {
	  if(json_object_get_type(objtor) == json_type_null)
              return empty;
          return json_object_get_string(objtor);
        }
    }
  return err;
}

const char * get_streams(json_object *j) //j is an array object
{
  char * err = "error";
  char * empty = "null";
  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      bool status;
      if((status = json_object_object_get_ex(lobj, "streams", &objtor)))
        {
	  if(json_object_get_type(objtor) == json_type_null)
            {
	      return empty;
	    }
          return json_object_get_string(objtor);
        }
    }
  return err;
}

const char * get_adserver_url(json_object *j) //j is an array object
{
  char * err = "error";
  char * empty = "null";
  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      bool status;
      if((status = json_object_object_get_ex(lobj, "adServerUrl", &objtor)))
        {
	  if(json_object_get_type(objtor) == json_type_null)
            {
	      //return json_type_to_name(json_type_null);
              //s << json_type_to_name(json_type_null);
	      //std::cout << "type to name " << s.str() << std::endl;
	      //std::string test = s.str();
              //return s.str();
	      return empty;
            }
	  //std::cout << "adserv: " << json_object_get_type(objtor) << std::endl;
          return json_object_get_string(objtor);
        }
    }
  return err;
}

const char * get_tags(json_object *j) //j is an array object
{
  char * err = "error";
  char * empty = "null";
  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      bool status;
      if((status = json_object_object_get_ex(lobj, "tags", &objtor)))
        {
	  if(json_object_get_type(objtor) == json_type_null)
	    {
            //return json_type_to_name(json_type_null);
	    //  s << json_type_to_name(json_type_null);
	    //  std::cout << "type to name " << s.str() << std::endl;
	    //  return s.str();
	      return empty;
	    }
	  //std::cout << "TYPE!!!!!!!!! " << json_object_get_type(objtor) << std::endl;
          return json_object_get_string(objtor);
        }
    }
  return err;
}

const char * get_priority(json_object *j) //j is an array object
{
  char * err = "error";
  char * empty = "null";
  //std::cout << "OBJ LEN IN PRI!!!!!!: " << json_object_array_length(j) << std::endl;
  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      bool status;
      if((status = json_object_object_get_ex(lobj, "priority", &objtor)))
        {
	  //std::cout << "PRIORITY FOUND!!!!!!!!!!!!!!!!!" << json_object_get_int(objtor)<<std::endl;
	  if(json_object_get_type(objtor) == json_type_null)
            {
	      return empty;
            }
          return json_object_get_string(objtor);
	  //s << json_object_get_int(objtor);
	  //std::cout << "print it " << s.str() << std::endl;
          //return s.str();
	}
    }
  return err;
}

const char * get_object_type(json_object *j) //j is an array object
{
  char * empty = "null";
  char * err = "error";
  //std::cout << "IN TYPE!!!!!!!!" << std::endl;
  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      bool status;
      if((status = json_object_object_get_ex(lobj, "objectType", &objtor)))
	{
	  if(json_object_get_type(objtor) == json_type_null)
	    return empty;
          return json_object_get_string(objtor);
        }
    }
  return err;
}

const char * get_thumb_attribs(json_object *j) //j is an array object
{
  char * err = "error";
  char * empty = "null";
  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      bool status;
      if((status = json_object_object_get_ex(lobj, "thumbAttrs", &objtor)))
        {
	  if(json_object_get_type(objtor) == json_type_null)
            return empty;
          return json_object_get_string(objtor);
        }
    }
  return err;
}

const char * get_object_attribs(json_object *j) //j is an array object
{
  char * err = "error";
  char * empty = "null";
  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      bool status;
      if((status = json_object_object_get_ex(lobj, "objectAttrs", &objtor)))
        {
	  if(json_object_get_type(objtor) == json_type_null)
            return empty;
          return json_object_get_string(objtor);
        }
    }
  return err;
}

const char * get_children(json_object *j) //j is an array object
{
  char * err = "error";
  char * empty = "null";
  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      bool status;
      if((status = json_object_object_get_ex(lobj, "children", &objtor)))
        {
	  if(json_object_get_type(objtor) == json_type_null)
            return empty;
          return json_object_get_string(objtor);
        }
    }
  return err;
}

const char * get_policy_name(json_object *j) //j is an array object
{
  char * err = "error";
  char * empty = "null";
  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      bool status;
      if((status = json_object_object_get_ex(lobj, "policyName", &objtor)))
        {
	  if(json_object_get_type(objtor) == json_type_null)
            return empty;
          return json_object_get_string(objtor);
        }
    }
  return err;
}

const char * get_key_server_url(json_object *j) //j is an array object
{
  char * err = "error";
  char * empty = "null";
  for(int i = 0; i < json_object_array_length(j); i++)
    {
      json_object *lobj = json_object_array_get_idx(j, i);
      json_object *objtor;
      bool status;
      if((status = json_object_object_get_ex(lobj, "keyServerUrl", &objtor)))
        {
	  if(json_object_get_type(objtor) == json_type_null)
            return empty;
          return json_object_get_string(objtor);
        }
    }
  return err;
}

#if 0
char * parse_provider_list(char * jstr)
{
  char * retval = "";
  json_object *jsonArray = json_tokener_parse(jstr);

  int length = json_object_array_length(jsonArray);
  for(int i = 0; i < length; i++)
  {
    json_object *lobj = json_object_array_get_idx(jsonArray, i);
    //printf(" provider_json : %s\n", json_object_get_string(lobj));
    json_object *providerJson = json_tokener_parse(json_object_get_string(lobj));

    json_object *name_tmp;
    json_object_object_get_ex(providerJson, "name", &name_tmp);
    const char * name = json_object_get_string(name_tmp);
    //std::cout<< "name:" << name  << std::endl;

    json_object *cp_tmp;
    json_object_object_get_ex(providerJson, "contentProvider", &cp_tmp);
    const char * cp =  json_object_get_string(cp_tmp);
    //std::cout<< "content provider:" << cp << std::endl;

    json_object *sub_tmp;
    json_object_object_get_ex(providerJson, "autoSubscribe", &sub_tmp);
    int sub = json_object_get_boolean(sub_tmp);
    printf("autoSub: %s\n", sub? "true": "false");

    retval += "(" + quotesql(name) + "," + quotesql(cp) + "," + std::to_string(sub) + ")";
    if(i != length-1){
      retval += ",";
    }
  }

  return retval;
}
#endif
