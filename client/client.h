#include <map>
#include <string>
#include "audio.h"


#define CACHE "../voice_data/"
#define SERVER "localhost 5000"
#define PORT "80"


class client
{
public:
  client(std::string directory, size_t device, size_t n_channels, size_t buffer_size, 
        size_t first_channel, size_t sample_rate);
  client(std::string directory);
  client();   
  void run();
private:
  bool status;
  void help();
  void show_base();
  void expand_base(std::vector<std::string> command);
  void off() {status =false;}
  void play(std::vector<std::string> command);
  void record(std::vector<std::string> command);
  void remove(std::vector<std::string> command);
  void push(std::string server, std::string port, std::string path);

  std::string cache_directory;
  std::map<std::string, std::string> local_base;
  audio::play speaker;
  audio::record microphone;
  
};