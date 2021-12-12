#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include "audio.h"
#include <filesystem>
#include <map>


class client
{
public:
  client(size_t device, size_t n_channels, size_t buffer_size, 
        size_t first_channel, size_t sample_rate);
  void run();
private:
  bool status;
  void help();
  void show_base();
  void expand_base(std::vector<std::string> command);
  void off() {status =false;}
  void play(std::vector<std::string> command);
  void record(std::vector<std::string> command);
  void time_record();
  void time_play();

  std::map<std::string, std::string> local_base;
  audio::play speaker;
  audio::record microphone;

};

client::client(size_t device, size_t n_channels, size_t buffer_size, 
        size_t first_channel, size_t sample_rate) 
        
  : speaker(device, n_channels, buffer_size, first_channel, sample_rate),
    microphone(device, n_channels, buffer_size, first_channel, sample_rate),
    status(true)
{};



void client::run() {
  std::cout<<"client started" <<std::endl;
  while(status){
    std::cout<<std::endl;
    std::cout<<"command:\t";
    std::vector<std::string> command;
    std::string str;
    std::getline(std::cin, str);
    size_t pos = 0;
    std::string token;
    while ((pos = str.find(' ')) != std::string::npos) {
      token = str.substr(0, pos);
      command.push_back(token);
      str.erase(0, pos + 1);
    }
    command.push_back(str);
    if (command[0] == "help") {
      help();
      continue;
    }
    if (command[0] == "show") {
      show_base();
      continue;
    }
    if (command[0] == "expand") {
      expand_base(command);
      continue;
    }
    if (command[0] == "close") {
      off();
      continue;
    }
    if (command[0] == "play") {
      play(command);
      continue;
    }
    if (command[0] == "record") {
      record(command);
      continue;
    }
    std::cout << "Incorrect command" << std::endl;
  }
}

void client::help() {
  std::cout << std::endl << std::endl;
  std::cout<<"expand local directory:\t\t\t\t expand your_directory" << std::endl;
  std::cout<<"record a voice message:\t\t\t\t record audio_message" << std::endl;
  std::cout<<"listen to the message:\t\t\t\t play audio_message" << std::endl;
  std::cout<<"send a message to the server:\t\t\t push audio_message" << std::endl;
  std::cout<<"request a list of messages from the server:\t request" << std::endl;
  std::cout<<"show messages in directory:\t\t\t show" << std::endl;
  std::cout<<"close application:\t\t\t\t close" << std::endl;
  std::cout<<std::endl;
}

void client::show_base() {
  for (auto it = local_base.begin(); it!=local_base.end(); it++) {
    std::cout<<"data: "<< it->first <<"\t"<< "path: " << it->second << std::endl;
  }
}

void client::expand_base(std::vector<std::string> command) {
  if (command.size() < 2) {
    std::cout<<"Incorrect input" << std::endl;
    return;
  }
  if (!std::filesystem::exists(command[1])) {
    std::cout << "No such directory" << std::endl;
    return;
  }
  for (const auto & entry : std::filesystem::directory_iterator(command[1])) {
    if (entry.path().extension() == ".raw") {
      local_base[entry.path().stem()] = entry.path();
    }
    
  }  
}

void client::play(std::vector<std::string> command) {
  if (command.size() < 2) {
    std::cout<<"Incorrect input" << std::endl;
    return;
  }
  if (!local_base.count(command[1])) {
    std::cout<<"There in no file with such a key. Try expand your_directory." <<std::endl;
    return;
  }
  if (command.size() == 3) {
    double time = std::stod(command[2]);
      if (time > 1 || time < 0) {
        std::cout<<"Incorrect time" <<std::endl;
        return;
      }
      speaker.set_file(local_base[command[1]]);
      speaker.set_time(time);
    } else {
      speaker.set_file(local_base[command[1]]);
    }
  std::cout << "Press enter to interrupt playback" << std::endl;  


  std::thread th([&](){
    speaker.play_file();
  });


  bool interrupt = false;
  std::thread th1([this, &interrupt](){
    while (!speaker.get_status()) {}  
    std::cout<<std::fixed<<std::setprecision(1);
    while (speaker.get_status()) { 
      std::cout<<"\r"<<"play_time: "<<speaker.current_time()<<"/"<<speaker.get_duration()<<std::flush;
      std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
    }
    std::cout<<std::endl;
    if (!interrupt) {
      std::cout<<"Press enter to continue..."<<std::flush;
    }
  });

  std::jthread th2([this, &interrupt] {
    getchar();
    interrupt = true;
    speaker.off();
  });


  th.join();
  th1.join();
  if(interrupt) {
    th2.join();
  } else {
    th2.request_stop();
  }
}




void client::record(std::vector<std::string> command) {
  if (command.size() < 2) {
    std::cout<<"Incorrect command" << std::endl;
    return;
  }
  std::string path;
  if (command.size() == 3) {
    path = command[2] + "/" + command[1] + ".raw";
  } else {
    path = "../voice_data/" + command[1] + ".raw";
  }
  std::cout << "Press enter to stop record" << std::endl;
  std::thread th([&](){
    microphone.input(path);
  });
  std::thread th1([&]() {
    time_record();
  });
  getchar();
  microphone.off();
  th.join();
  th1.join();
  local_base[command[1]] = path;
}

void client::time_record() {
  while (!microphone.get_start_record()) {}  
  std::cout<<std::fixed<<std::setprecision(1);
  while (microphone.get_status()) { 
    std::cout<<"\r"<<"recording time: "<<microphone.stream_time()<<std::flush;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}



int main()
{
    client cl(0, 1,1024, 0, 44100);
    cl.run();


  return 0;
}