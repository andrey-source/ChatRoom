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
  void expand_base(std::string path);
  void off() {status =false;}
  void play(std::vector<std::string> command);
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
      if (command.size() < 2) {
        std::cout<<"incorrect input" << std::endl;
        continue;
      }
      expand_base(command[1]);
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


    std::cout << "incorrect command" << std::endl;
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

void client::expand_base(std::string path) {
  if (!std::filesystem::exists(path)) {
    std::cout << "No such directory" << std::endl;
    return;
  }
  for (const auto & entry : std::filesystem::directory_iterator(path)) {
    if (entry.path().extension() == ".raw") {
      local_base[entry.path().stem()] = entry.path();
    }
    
  }  
}

void client::play(std::vector<std::string> command) {
  if (command.size() == 3) {
    double time = std::stod(command[2]);
      if (time > 1 || time < 0) {
        std::cout<<"incorrect time" <<std::endl;
        return;
      }
      speaker.set_time(time);
    }
  speaker.set_file(local_base[command[1]]);
  std::thread th([&](){
    speaker.play_file();
  });
  th.detach();
}



  // first run
//   audio::record rec(0, 1,1024, 0, 44100);
//   bool test;
//   std::thread t1_1([&]()
//   {
//     test = rec.input("../voice_data/test1.raw");
//   });
//   std::this_thread::sleep_for(std::chrono::milliseconds(5000));
//   rec.off();
//   t1_1.join();


class server
{
  public:
  void expand_base(std::string path);
  void show_base();

  private:

  std::map<std::string, std::string> base;
};



void server::expand_base(std::string path) {
  for (const auto & entry : std::filesystem::directory_iterator(path)) {
    if (entry.path().extension() == ".raw") {
      base[entry.path().stem()] = entry.path();
    }
    
  }  
}

void server::show_base() {
  for (auto it = base.begin(); it!=base.end(); it++) {
    std::cout<<"data: "<< it->first <<"\t"<< "path: " << it->second << std::endl;
  }
}






int main()
{
  // first run
//   audio::record rec(0, 1,1024, 0, 44100);
//   bool test;
//   std::thread t1_1([&]()
//   {
//     test = rec.input("../voice_data/test1.raw");
//   });
//   std::this_thread::sleep_for(std::chrono::milliseconds(5000));
//   rec.off();
//   t1_1.join();

  // audio::play pl(0, 1,1024, 0, 44100);  
  // pl.set_file("../voice_data/test2.raw");
  // pl.set_time(0);
  // std::thread t1_2([&]()
  // {
  //   pl.play_file();
  // });
  // std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  // // pl.set_time();
  // t1_2.join();



// //   second run
//   std::thread t2_1([&]()
//   {
//     test = rec.input("../voice_data/test2.raw");
//   });
//   std::this_thread::sleep_for(std::chrono::milliseconds(5000));
//   rec.off();
//   t2_1.join();

//   pl.set_file("../voice_data/test2.raw");
//   pl.set_time(0.5);

//   std::thread t2_2([&]()
//   {
//     pl.play_file();
//   });
//   std::this_thread::sleep_for(std::chrono::milliseconds(2000));
//   pl.off();
//   t2_2.join();



    std::string puth = "../voice_data";
    std::map<std::string, std::string> map;


    //  entry.path()<<std::endl;

    // std::cout<< puths.extact("../voice_data/test2.raw");
    // server test;
    // test.expand_base("../voice_data");
    // test.show_base();


    // std::vector<std::string> test;
    // std::string str;
    // std::getline(std::cin, str);
    // size_t pos = 0;
    // std::string token;
    // while ((pos = str.find(' ')) != std::string::npos) {
    //   token = str.substr(0, pos);
    //   test.push_back(token);
    //   str.erase(0, pos + 1);

    // }
    // test.push_back(str);



    // for (int i = 0; i < test.size(); i++) {
    //   std::cout<<test[i] << std::endl;
    // }

    client cl(0, 1,1024, 0, 44100);
    cl.run();

  return 0;
}