#include <map>
#include <string>
#include "audio.h"


#define CACHE "../voice_data/"
#define SERVER "localhost 5000"
#define PORT "80"


class client
{
public:
  client();   
  client(std::string server, std::string port);
  
  void run();
private:
  bool status;
  void help();
  void ls();
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