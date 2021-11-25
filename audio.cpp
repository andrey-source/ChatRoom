#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <algorithm>
#include <thread>
#include <unistd.h>
#include <string>





#define INITIAL_CAPASITY 44100
#define SAMPLE_RATE 44100
#define BUFFER_SIZE 4096
#define N_CNAHHELS 2
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
    parameters.deviceId = adc.getDefaultInputDevice();
    parameters.nChannels = N_CNAHHELS;
    butch_size = BUFFER_SIZE;
    start_flag = false;
    status = false;
  }
  bool input(std::string puth);
  void off();  
  void set_config();
private:
  bool status;
  bool start_flag;
  static int recording_butch(void * /*outputBuffer*/, void *InputBuffer, unsigned int nBufferFrames,
         double /*streamTime*/, RtAudioStreamStatus /*status*/, void *userData );
  inline static RtAudio::StreamParameters parameters; 
  unsigned int butch_size;
  RtAudio adc;
};

void record::off() {
  while (!start_flag) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    status = false;
}

int record::recording_butch(void * /*outputBuffer*/, void *InputBuffer, unsigned int nBufferFrames,
         double /*streamTime*/, RtAudioStreamStatus /*status*/, void *userData) {
  std::ofstream *file = (std::ofstream*)userData;
  file->write((const char*)InputBuffer, sizeof(MY_TYPE) * N_CNAHHELS * nBufferFrames);
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
  unsigned int butch_size = BUFFER_SIZE;
  try {
    adc1.openStream(nullptr, &parameters, FORMAT,
                    SAMPLE_RATE, &butch_size, &this->recording_butch, (void*)&file);
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
  return true;
}


class play
{
  public:
  play() {
    parameters.deviceId = dac.getDefaultOutputDevice();
    parameters.nChannels = N_CNAHHELS;
    butch_size = BUFFER_SIZE;
  }
  bool output(std::string puth);
private:
  static int play_butch(void *OutputBuffer,  void */*InputBuffer*/, unsigned int nBufferFrames,
         double /*streamTime*/, RtAudioStreamStatus /*status*/, void *userData );
  unsigned int butch_size;
  static inline RtAudio::StreamParameters parameters;
  RtAudio dac;
};

int play::play_butch(void *OutputBuffer,  void */*InputBuffer*/, unsigned int nBufferFrames,
         double /*streamTime*/, RtAudioStreamStatus /*status*/, void *userData ) {
    std::ifstream *file = (std::ifstream *)userData;
    file->read((char*)OutputBuffer, sizeof(MY_TYPE) * N_CNAHHELS * BUFFER_SIZE);
    if(file->eof()){
      return 1;
    }
    return 0;
}

bool play::output(std::string puth){

  std::ifstream file(puth, std::ios::binary);
  try {
    dac.openStream(&parameters, nullptr,  FORMAT,
                    SAMPLE_RATE, &butch_size, &play_butch, (void*)&file);
    dac.startStream();
  }
  catch ( RtAudioError& e ) {
    e.printMessage();
    exit( 0 );
  }
  while ( true ) {
    // SLEEP( 100 ); // wake every 100 ms to check if we're done
    if ( dac.isStreamRunning() == false ) break;
  }
  try {
    dac.stopStream();
  }
  catch (RtAudioError& e) {
    e.printMessage();
  }
  if ( dac.isStreamOpen() ) dac.closeStream();
  file.close();
  return true;
}



int main() 
{
  record rec;
  bool test;
  std::thread t([&]()
  {
    test = rec.input("test1.raw");
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  rec.off();
  t.join();

  play pl;
  pl.output("test1.raw");

    std::thread t1([&]()
  {
    test = rec.input("test2.raw");
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  rec.off();
  t1.join();

  pl.output("test2.raw");

  return 0;
}