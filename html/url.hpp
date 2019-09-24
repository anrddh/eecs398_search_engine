#include <string>
#include <iostream>

namespace fb{

class url {
private:
   std::string protocol;
   std::string hostname;
   std::string file_path; //file path includes the initial /
                          //i.e. for eecs398.com/project, file path is /project
public:
   
   //Creates a url object from url
   url(const std::string &in_url){
      int index = 0;
      for ( ; in_url[index] != ':'; ++index){
         protocol += in_url[index];
      }
      index += 3; //skip over the "://"
      for ( ; in_url[index] != '/'; ++index){
         hostname += in_url[index];
      }
      for ( ; index < in_url.size(); ++index){
         file_path += in_url[index];
      }
   }
   
   //Creates a url object from url fields
   //Can handle in_file_path with or without leading /
   url(const std::string in_protocol, const std::string in_hostname, const std::string in_file_path){
      protocol = in_protocol;
      hostname = in_hostname;
      if (in_file_path.size() > 0){
         if (in_file_path[0] != '/') file_path += '/';
      }
      file_path += in_file_path;
   }
   
   //Creates a url object by appending file path to existing url
   //Can handle in_file_path with or without leading /
   url (const url& current_url, const std::string in_file_path){
      protocol = current_url.protocol;
      hostname = current_url.hostname;
      file_path = current_url.file_path;
      if (in_file_path.size() > 0){
         if (in_file_path[0] != '/') file_path += '/';
      }
      file_path += in_file_path;
   }

   friend std::ostream &operator << (std::ostream &out, const url &url);
   //friend istream &operator >> (std::istream &in, url &url);
};

   std::ostream &operator << (std::ostream &out, const url &url){
      out << url.protocol << "://" << url.hostname << url.file_path;
   }

   //std::istream &operator >> (std::istream &in, url &url){}
}