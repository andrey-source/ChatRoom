#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <filesystem>
#include <cstdio>
#include "application.h"
#include "async.http.client.h"




application::application()
  : status(true), server(SERVER), port(PORT), cache_directory(CACHE){}

application::application(std::string server, std::string port)
  : status(true), server(server), port(port), cache_directory(CACHE){}





void application::run() {
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
    if (command[0] == "ls") {
      ls();
      continue;
    }
    if (command[0] == "open") {
      if (command.size() < 2) {
        std::cout << "Incorrect input" << std::endl;
        continue;
      }
      open(command[1]);
      continue;
    }
    if (command[0] == "close") {
      off();
      continue;
    }
    if (command[0] == "play") {
      handler_play(command);
      continue;
    }
    if (command[0] == "record") {
      handler_record(command);
      continue;
    }
    if (command[0] == "remove") {
        if (command.size() < 2) {
        std::cout<<"Incorrect input" << std::endl;
        continue;
        }
      if (remove(command[1])) {
        std::cout << "file with key " << command[1] << " deleted" << std::endl;
        continue;
      };
      std::cout << "file delection error" << std::endl;
      continue;
    }
    if (command[0] == "push") { //LIZA
      push(command);
      continue;
    }
    if (command[0] == "download") {  //LIZA
      download(command);
      continue;
    }
    if (command[0] == "show") {  // LIZA
      show_server(command);
      continue;
    }
    std::cout << "Incorrect command" << std::endl;
  }
}


void application::help() {
  std::cout << std::endl << std::endl;
  std::cout<<"action:\t\t\t\t\t\t command:"<<std::endl<<std::endl;
  std::cout<<"open directory:\t\t\t\t\t open your_directory" << std::endl;
  std::cout<<"record a voice message:\t\t\t\t record audio_message" << std::endl;
  std::cout<<"listen to the message:\t\t\t\t play audio_message" << std::endl;
  std::cout<<"send a message to the server:\t\t\t push audio_message" << std::endl;
  std::cout<<"request a list of messages from the server:\t show" << std::endl;
  std::cout<<"remove file:\t\t\t\t\t remove your_file" << std::endl;
  std::cout<<"show audio files in current directory:\t\t ls" << std::endl;
  std::cout<<"close application:\t\t\t\t close" << std::endl;
  std::cout<<std::endl;
}


void application::ls() {
  open(cache_directory);
  for (auto it = local_base.begin(); it!=local_base.end(); it++) {
    std::cout<<"key: "<< it->first <<"\t"<< "path: " << it->second << std::endl;
  }
}

void application::update(std::string path, std::string extension) {
  for (const auto & entry : std::filesystem::directory_iterator(path)) {
    if (entry.path().extension() == extension) {
      local_base[entry.path().stem()] = entry.path();
    } 
  }  
}

void application::open(std::string path) { 
  if (path[path.size() - 1] != '/') {
    path +='/';
  }
  try {
    if (!std::filesystem::exists(path)) {
      cache_directory = path;
      std::filesystem::create_directories(path);
      return;
  }
    cache_directory = path;
    local_base.clear();
    update(path, ".wav");
    update(path, ".mp3");
    update(path, ".cd"); 
  } catch (std::exception &e) {
    std::cout << "Incorrect input" << std::endl;
    }
  }


bool application::remove(std::string key) {
  if (std::filesystem::remove(local_base[key])) {
    local_base.erase(key);
    return true;
  }
  return false;
}

void application::handler_play(std::vector<std::string> command) {
  if (command.size() < 2 || command.size() > 3) {
    std::cout<<"Incorrect input" << std::endl;
    return;
  }
  if (!local_base.count(command[1])) {
    std::cout<<"There in no file with such a key. Try again." <<std::endl;
    return;
  }
  double time = 0;
  if (command.size() == 3) {
    try {
      time = std::stod(command[2]);
    } catch (std::invalid_argument const& ex){
      std::cout << "invalid time" << std::endl;
      return;
    }
  }
  if (time >1 || time < 0) {
    std::cout<<"invalid range try from 0 to 1" << std::endl;
    return;
  }
  play(local_base[command[1]], time);
}


void application::play(std::string path, double time) {
  open(cache_directory);
  audio::play speaker;
  speaker.set_file(path);
  speaker.set_time(time);
  std::string extension = std::filesystem::path(path).extension();
  if (extension == ".mp3") {
    speaker.set_config(0, 2, 1024, 0, 48000);
  }
  if (extension == ".wav") {
    speaker.set_config(0, 2, 1024, 0, 44100);
  }
  if (extension == ".cd") {
    speaker.set_config(0, 2, 1024, 0, 44100);
  }
  std::cout << "Press enter to interrupt playback" << std::endl;  
  std::thread th([&](){
    speaker.play_file();
  });

  bool interrupt = false;
  std::thread th1([&speaker, &interrupt](){
    while (!speaker.get_status()) {}  
    std::cout<<std::fixed<<std::setprecision(1);
    while (speaker.get_status()) { 
      std::cout<<"\r"<<"play time: "<<speaker.current_time()<<"/"<<speaker.get_duration()<<std::flush;
      std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
    }
    std::cout<<std::endl;
    if (!interrupt) {
      std::cout<<"Press enter to continue..."<<std::flush;
    }
  });
  std::jthread th2([&speaker, &interrupt] {
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



void application::handler_record(std::vector<std::string> command) {
  open(cache_directory);
  if (command.size() < 2 || command.size() > 3) {
    std::cout<<"Incorrect command" << std::endl;
    return;
  }
  if (local_base.count(command[1])) {
    std::cout<<"A file with such a key already exists" << std::endl;
    return; 
  }
  std::string extension = ".wav";
  if (command.size() == 3) {
    if (command[2] == "cd") {
    } else if (command[2] == "mp3") {
      extension = ".mp3";
    } else if (command[2] == "wav") {
      extension = ".wav";
    } else {
      std::cout<<"Incorrect extension" << std::endl;
      return;
    }
  }
  std::string path = cache_directory + command[1] + extension;

  record(path);
}


void application::record(std::string path) {
  std::string extension = std::filesystem::path(path).extension();
  audio::record microphone;
  if (extension == ".mp3") {
    microphone.set_config(0, 2, 1024, 0, 48000);
  }
  if (extension == ".wav") {
    microphone.set_config(0, 2, 1024, 0, 44100);
  }
  if (extension == ".cd") {
    microphone.set_config(0, 2, 1024, 0, 44100);
  }
  std::cout << "Press enter to stop record" << std::endl;
  std::thread th([&](){
    microphone.input(path);
  });
  std::thread th1([&microphone]() {
    while (!microphone.get_start_record()) {}  
    std::cout<<std::fixed<<std::setprecision(1);
    while (microphone.get_status()) { 
      std::cout<<"\r"<<"recording time: "<<microphone.stream_time()<<std::flush;
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  });
  getchar();
  microphone.off();
  th.join();
  th1.join();
  local_base[std::filesystem::path(path).stem()] = path;
}




 // LIZA
void application::show_server(std::vector<std::string> command) {
  net::io_context io_context;
  std::make_shared<client::Client>(io_context)->show_server(server, port);
  io_context.run();
};

 //LIZA
void application::push(std::vector<std::string> command) {
  net::io_context io_context;
  std::make_shared<client::Client>(io_context)->push(server, port, local_base[command[1]]);
  io_context.run();
}

void application::download(std::vector<std::string> command) {
  net::io_context io_context;
  std::string path = cache_directory + command[1] + ".wav";
  std::make_shared<client::Client>(io_context)->download(server, port, command[1], path);
  io_context.run();
}
