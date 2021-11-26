#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <thread>
#include <unistd.h>
#include <string>
#include <experimental/filesystem>





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
    bool flag_on = false;
    set_config(0, N_CNAHHELS, BUFFER_SIZE, FIRTS_CHANNEL, SAMPLE_RATE);
  }
  record(size_t device, size_t n_channels, unsigned int buffer_size, size_t first_channel, size_t Sample_rate) {
    start_flag = false;
    status = false;
    set_config(device, n_channels, buffer_size, first_channel, Sample_rate);
  }

  bool input(std::string  puth);
  void off() {while (!start_flag){} status=false;};  
  void on() {}
  void set_config(size_t device, size_t n_channels, unsigned int buffer_size, size_t first_channel, size_t Sample_rate);
private:
  bool status;
  bool start_flag;
  bool flag_on;
  static int recording_butch(void * /*outputBuffer*/, void *InputBuffer, unsigned int nBufferFrames,
         double /*streamTime*/, RtAudioStreamStatus /*status*/, void *userData );
  inline static RtAudio::StreamParameters parameters; 
  unsigned int buffer_size;
  size_t sample_rate;
  RtAudio adc;
};

void record::set_config(size_t device, size_t n_channels, unsigned int Buffer_size,
 size_t first_channel, size_t Sample_rate) {
  if (!device) {
    parameters.deviceId = adc.getDefaultInputDevice();
  } else {
    parameters.deviceId = device;
  }
  parameters.nChannels = n_channels;
  parameters.firstChannel = first_channel;
  buffer_size = Buffer_size;
  sample_rate = Sample_rate;
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
                    sample_rate, &buffer_size, &this->recording_butch, (void*)&file);
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
  std::cout << std::endl;
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
    off_flag = false;
    on_flag = false;
    status = false;
    current_butch = 0;
    set_config(0, N_CNAHHELS, BUFFER_SIZE, FIRTS_CHANNEL, SAMPLE_RATE);
  }
  play(size_t device, size_t n_channels, unsigned int buffer_size, size_t first_channel, size_t sample_rate) {
    status = true;
    off_flag = false;
    on_flag = false;
    current_butch = 0;
    last_butch = 0;
    set_config(device, n_channels, buffer_size, first_channel, sample_rate);
  }
   
  void on() {on_flag = true;}
  void off() {off_flag = true;}
  void close() {off_flag = true; status = false; on_flag = false;};
  bool check_progress() {return false;};
  double duratin(std::string);

  void set_time() {current_butch = 100;};

  bool output(std::string puth);
  void set_config(size_t device, size_t n_channels, unsigned int Buffer_size, size_t first_channel, size_t sample_rate);
private:
  static int play_butch(void *OutputBuffer,  void */*InputBuffer*/, unsigned int nBufferFrames,
         double /*streamTime*/, RtAudioStreamStatus /*status*/, void *userData );
  unsigned int buffer_size;
  size_t sample_rate;
  static inline RtAudio::StreamParameters parameters;
  size_t progress;
  static inline size_t current_butch;
  size_t last_butch;
  bool status;  
  bool off_flag;
  bool on_flag;
  RtAudio dac;
};

double play::duratin(std::string file_name) {
  std::ifstream file(file_name, std::ios::binary | std::ios::ate);
  size_t n_bytes = file.tellg();
  double time = n_bytes / parameters.nChannels / sizeof(MY_TYPE) / sample_rate;
  file.close();
  return time;
}

void play::set_config(size_t device, size_t n_channels, unsigned int Buffer_size,
 size_t first_channel, size_t Sample_rate) {
  if (!device) {
    parameters.deviceId = dac.getDefaultOutputDevice();
  } else {
    parameters.deviceId = device;
  }
  sample_rate = Sample_rate;
  parameters.nChannels = n_channels;
  parameters.firstChannel = first_channel;
  buffer_size = Buffer_size;
}

int play::play_butch(void *OutputBuffer,  void */*InputBuffer*/, unsigned int nBufferFrames,
         double /*streamTime*/, RtAudioStreamStatus /*status*/, void *userData ) {
    std::ifstream *file = (std::ifstream *)userData;

    file->seekg(current_butch * sizeof(MY_TYPE) * parameters.nChannels * nBufferFrames, std::ios_base::beg);
    file->read((char*)OutputBuffer, sizeof(MY_TYPE) * parameters.nChannels * nBufferFrames);
    current_butch++;
    std::cout << "current_butch: " << current_butch << std::endl;
    auto read_pos = file->tellg();
    // std::cout << read_pos << std::endl;
    if(file->eof()){
      return 1;
    }
    return 0;
}

bool play::output(std::string puth){
  std::ifstream file(puth, std::ios::binary | std::ios::ate);
  size_t n_bytes = file.tellg();
  last_butch = n_bytes / parameters.nChannels / sizeof(MY_TYPE) / buffer_size;
  file.seekg(0);
  try {
    dac.openStream(&parameters, nullptr,  FORMAT,
                    sample_rate, &buffer_size, &play_butch, (void*)&file);
    // dac.startStream();
  }
  catch ( RtAudioError& e ) {
    e.printMessage();
    exit( 0 );
  }
  while(status) {
    if (on_flag) {
      on_flag = false;
      try {
        dac.startStream();
      }
      catch ( RtAudioError& e ) {
      dac.closeStream();
      e.printMessage();
      exit( 0 );
      }
    }
    while (dac.isStreamRunning()) {
      if (off_flag) {
        dac.stopStream();
        break;
      }
    }
  }
  if ( dac.isStreamOpen() ) dac.closeStream();
  file.close();
  return true;
}



int main() 
{

  // first run
  record rec(0, 1,1024, 0, 44100);
  bool test;
  std::thread t1_1([&]()
  {
    test = rec.input("voice_data\\test1.raw");
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  rec.off();
  t1_1.join();

  play pl(0, 1,1024, 0, 44100);

  std::cout << "duration: " << pl.duratin("voice_data\\test1.raw")<<std::endl;
  pl.on();
  std::thread t1_2([&]()
  {
    pl.output("voice_data\\test1.raw");
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  pl.set_time();
  pl.on();
  t1_2.join();







  // second run
  // std::thread t2_1([&]()
  // {
  //   test = rec.input("voice_data\\test2.raw");
  // });
  // std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  // rec.off();
  // t2_1.join();

  // std::thread t2_2([&]()
  // {
  //   pl.output("voice_data\\test2.raw");
  // });
  // std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  // pl.off();
  // t2_2.join();

  return 0;
}