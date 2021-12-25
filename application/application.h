#include <map>
#include <string>
#include "audio.h"


#define CACHE "voice_data"
#define PUSH_CACHE "/push_cache//"
#define DOWNLOAD_CACHE "/download_cache//"
#define SERVER "localhost"
#define PORT "5000"
#define CHUNCK_SIZE 524288 // 512 kb


class application
{
public:
  application();   
  application(std::string server, std::string port);
  
  void run();
private:
  bool status;
  void help();
  void ls();
  void rename(std::vector<std::string> command);
  void open(std::string path);
  void off() {status =false;}
  void play(std::string path, double time);
  void record(std::string path);
  void handler_play(std::vector<std::string> command);
  void handler_record(std::vector<std::string> command);

  
  bool remove(std::string key);

  void push(std::vector<std::string> command);  //LIZA
  void download(std::vector<std::string> command); // LIZA
  void show_server(std::vector<std::string> command); // LIZA




  void update(std::string path, std::string extension);

  std::string cache_directory;
  std::map<std::string, std::string> local_base;


  std::string server; // LIZA
  std::string port; // LIZA

  
};
