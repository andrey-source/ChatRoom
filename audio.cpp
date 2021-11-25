#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <thread>
#include <unistd.h>
#include <string>
#include <filesystem>





#define INITIAL_CAPASITY 44100
#define SAMPLE_RATE 44100
#define BUFFER_SIZE 4096
#define N_CNAHHELS 2
#define FIRTS_CHANNEL 0
#define FORMAT RTAUDIO_SINT16
typedef signed short MY_TYPE;

 #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )


struct io
{
  FILE *file;
  size_t n_channels;
};


class record
{
public:
  record() {
    start_flag = false;
    status = false;
    set_config(0, N_CNAHHELS, BUFFER_SIZE, FIRTS_CHANNEL);
  }
  record(size_t device, size_t n_channels, unsigned int buffer_size, size_t first_channel) {
    start_flag = false;
    status = false;
    set_config(device, n_channels, buffer_size, first_channel);
  }
  bool input(std::string  puth);
  void off() {while (!start_flag){} status=false;};  
  void set_config(size_t device, size_t n_channels, unsigned int buffer_size, size_t first_channel);
private:
  bool status;
  bool start_flag;
  static int recording_butch(void * /*outputBuffer*/, void *InputBuffer, unsigned int nBufferFrames,
         double /*streamTime*/, RtAudioStreamStatus /*status*/, void *userData );
  inline static RtAudio::StreamParameters parameters; 
  unsigned int buffer_size;
  RtAudio adc;
};

void record::set_config(size_t device, size_t n_channels, unsigned int Buffer_size , size_t first_channel) {
  if (!device) {
    parameters.deviceId = adc.getDefaultInputDevice();
  } else {
    parameters.deviceId = device;
  }
  parameters.nChannels = n_channels;
  parameters.firstChannel = first_channel;
  buffer_size = Buffer_size;
}


int record::recording_butch(void * /*outputBuffer*/, void *InputBuffer, unsigned int nBufferFrames,
         double /*streamTime*/, RtAudioStreamStatus /*status*/, void *userData) {
  std::ofstream *file = (std::ofstream*)userData;
  file->write((const char*)InputBuffer, sizeof(MY_TYPE) * parameters.nChannels * nBufferFrames);
  return 0;
}

bool record::input(std::string puth) {
  start_flag = true;
  status = true;
  std::ofstream file(puth, std::ios::binary);
  RtAudio adc1;
  if ( adc1.getDeviceCount() < 1 ) {
    std::cout << "\nNo audio devices found!\n";
    exit( 0 );
  }
  try {
    adc1.openStream(nullptr, &parameters, FORMAT,
                    SAMPLE_RATE, &buffer_size, &this->recording_butch, (void*)&file);
    adc1.startStream();
  }
  catch ( RtAudioError& e ) {
    e.printMessage();
    exit( 0 );
  }
  while (status) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << status <<std::flush;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  try {
    adc1.stopStream();
  }
  catch (RtAudioError& e) {
    e.printMessage();
  }
  if (adc1.isStreamOpen()) {
    adc1.closeStream();
  }
  file.close();
  start_flag = false;
  return true;
}

class play
{
  public:
  play() {
    stop = false;
    start_flag = false;
    set_config(0, N_CNAHHELS, BUFFER_SIZE, FIRTS_CHANNEL);
  }
  play(size_t device, size_t n_channels, unsigned int buffer_size, size_t first_channel) {
    stop = false;
    start_flag = false;
    set_config(device, n_channels, buffer_size, first_channel);
  }
  void off() {while (!start_flag){} stop=true;};  
  bool output(std::string puth);
  void set_config(size_t device, size_t n_channels, unsigned int Buffer_size, size_t first_channel);

private:
  static int play_butch(void *OutputBuffer,  void */*InputBuffer*/, unsigned int nBufferFrames,
         double /*streamTime*/, RtAudioStreamStatus /*status*/, void *userData );
  unsigned int buffer_size;
  static inline RtAudio::StreamParameters parameters;
  bool stop;
  bool start_flag;
  RtAudio dac;
};

void play::set_config(size_t device, size_t n_channels, unsigned int Buffer_size, size_t first_channel) {
  if (!device) {
    parameters.deviceId = dac.getDefaultOutputDevice();
  } else {
    parameters.deviceId = device;
  }
  parameters.nChannels = n_channels;
  parameters.firstChannel = first_channel;
  buffer_size = Buffer_size;
}

int play::play_butch(void *OutputBuffer,  void */*InputBuffer*/, unsigned int nBufferFrames,
         double /*streamTime*/, RtAudioStreamStatus /*status*/, void *userData ) {
    std::ifstream *file = (std::ifstream *)userData;
    file->read((char*)OutputBuffer, sizeof(MY_TYPE) * parameters.nChannels * nBufferFrames);
    if(file->eof()){
      return 1;
    }
    return 0;
}

bool play::output(std::string puth){
  std::ifstream file(puth, std::ios::binary);
  start_flag = true;
  try {
    dac.openStream(&parameters, nullptr,  FORMAT,
                    SAMPLE_RATE, &buffer_size, &play_butch, (void*)&file);
    dac.startStream();
  }
  catch ( RtAudioError& e ) {
    e.printMessage();
    exit( 0 );
  }
  while (dac.isStreamRunning() && !stop) {
    // if ( dac.isStreamRunning() == false ) break;
  }
  try {
    dac.stopStream();
  }
  catch (RtAudioError& e) {
    e.printMessage();
  }
  if ( dac.isStreamOpen() ) dac.closeStream();
  file.close();
  stop = false;
  start_flag = false;
  return true;
}



int main() 
{

  // first run
  record rec(0, 1,1024, 0);
  bool test;
  std::thread t1_1([&]()
  {
    test = rec.input("voice_data\\test1.raw");
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  rec.off();
  t1_1.join();

  play pl(0, 1,1024, 0);
  std::thread t1_2([&]()
  {
    pl.output("voice_data\\test1.raw");
  });
  t1_2.join();







  // second run
  std::thread t2_1([&]()
  {
    test = rec.input("voice_data\\test2.raw");
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  rec.off();
  t2_1.join();

  std::thread t2_2([&]()
  {
    pl.output("voice_data\\test2.raw");
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  pl.off();
  t2_2.join();


  return 0;
}