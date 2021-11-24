#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <algorithm>
#include <thread>
#include <unistd.h>



#define INITIAL_CAPASITY 44100
#define SAMPLE_RATE 44100
#define BUFFER_SIZE 512
#define N_CNAHHELS 2
#define FORMAT RTAUDIO_SINT16
typedef signed short MY_TYPE;

 #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )


class record
{
public:
  record() {
    std::cout << "record()"; 
    parameters.deviceId = adc.getDefaultInputDevice();
    parameters.nChannels = N_CNAHHELS;
    butch_size = BUFFER_SIZE;
  }
  bool input(const char *puth);
  void off() {status = false;}
  
private:
  bool status;
  static int recording_butch(void * /*outputBuffer*/, void *InputBuffer, unsigned int nBufferFrames,
         double /*streamTime*/, RtAudioStreamStatus /*status*/, void *userData );
  unsigned int butch_size;
  inline static RtAudio::StreamParameters parameters;
  RtAudio adc;
};

int record::recording_butch(void * /*outputBuffer*/, void *InputBuffer, unsigned int nBufferFrames,
         double /*streamTime*/, RtAudioStreamStatus /*status*/, void *userData) {
  FILE *file  = (FILE*)userData;
  size_t count = fwrite(InputBuffer, sizeof(MY_TYPE) * parameters.nChannels, nBufferFrames, file);
  if (count < nBufferFrames) {
    return 1;
  }
  return 0;
}

bool record::input(const char *puth) {
  FILE *file;
  status = true;
  std::cout << "input";
  file = fopen(puth, "wb");
  try {
    adc.openStream( NULL, &parameters, FORMAT,
                    SAMPLE_RATE, &butch_size, &recording_butch, (void*)&file);
    adc.startStream();
  }
  catch ( RtAudioError& e ) {
    e.printMessage();
    exit( 0 );
  }
  while (true) {
    if (!status) {
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::cout << "while";
    // SLEEP(100);
  }
  std::cout << "off while";
  try {
    // Stop the stream
    adc.stopStream();
  }
  catch (RtAudioError& e) {
    e.printMessage();
  }
  if ( adc.isStreamOpen() ) adc.closeStream();
  fclose(file);
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
  bool output(const char *puth);
private:
  static int play_butch(void *OutputBuffer,  void */*InputBuffer*/, unsigned int nBufferFrames,
         double /*streamTime*/, RtAudioStreamStatus /*status*/, void *userData );
  unsigned int butch_size;
  static inline RtAudio::StreamParameters parameters;
  RtAudio dac;
};

int play::play_butch(void *OutputBuffer,  void */*InputBuffer*/, unsigned int nBufferFrames,
         double /*streamTime*/, RtAudioStreamStatus /*status*/, void *userData ) {
    FILE *file  = (FILE*)userData;
    size_t count = fread(OutputBuffer, sizeof(MY_TYPE) * parameters.nChannels, nBufferFrames , file);
    if (count < nBufferFrames) {
        size_t bytes = (nBufferFrames - count) * parameters.nChannels * sizeof(MY_TYPE);
        size_t start_byte = count * parameters.nChannels * sizeof(MY_TYPE);
        memset((char*)(OutputBuffer) + start_byte, 0, bytes);
        return 1;
    }
    return 0;
}

bool play::output(const char *puth){
  FILE *file;
  file = fopen(puth, "rb");
  try {
    dac.openStream(&parameters, NULL,  FORMAT,
                    SAMPLE_RATE, &butch_size, &play_butch, (void*)&file);
    dac.startStream();
  }
  catch ( RtAudioError& e ) {
    e.printMessage();
    exit( 0 );
  }

  while ( true ) {
    SLEEP( 100 ); // wake every 100 ms to check if we're done
    if ( dac.isStreamRunning() == false ) break;
  }
  try {
    // Stop the stream
    dac.stopStream();
  }
  catch (RtAudioError& e) {
    e.printMessage();
  }
  if ( dac.isStreamOpen() ) dac.closeStream();
  return true;
}



int main() 
{
  record rec;
  bool test;
  std::thread t([&]()
  {
    // std::cout << "thread";
    test = rec.input("test.raw");
  });
  rec.off();
  t.join();

  // rec.input("test.raw");
  // int n;
  // rec.off();

  // play pl;
  // pl.output("test.raw");
  return 0;
}