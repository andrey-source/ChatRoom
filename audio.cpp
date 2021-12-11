
#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <thread>
#include <unistd.h>
#include <string>
#include <experimental/filesystem>
#include "audio.h"


namespace audio
{
  record::record(size_t device, size_t n_channels, unsigned int buffer_size, size_t first_channel, size_t Sample_rate) {
    start_flag = false;
    status = false;
    set_config(device, n_channels, buffer_size, first_channel, Sample_rate);
  }
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

void record::off() {while (!start_flag){} status=false;};
double record::stream_time() {return adc.getStreamTime();};

int record::recording_butch(void * /*outputBuffer*/, void *InputBuffer, unsigned int nBufferFrames,
         double /*streamTime*/, RtAudioStreamStatus /*status*/, void *userData) {
  std::ofstream *file = (std::ofstream*)userData;
  file->write((const char*)InputBuffer, sizeof(MY_TYPE) * parameters.nChannels * nBufferFrames);
  return 0;
}

bool record::input(std::string puth) {
  // start_flag = true;
  // status = true;
  std::ofstream file(puth, std::ios::binary);
  if ( adc.getDeviceCount() < 1 ) {
    std::cout << "\nNo audio devices found!\n";
    exit( 0 );
  }
  try {
    adc.openStream(nullptr, &parameters, FORMAT,
                    sample_rate, &buffer_size, &this->recording_butch, (void*)&file);
    adc.startStream();
  }
  catch ( RtAudioError& e ) {
    e.printMessage();
    exit( 0 );
  }
  status = true;
  start_flag = true;
  while (status) {
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // std::cout << status <<std::flush;
  }
  // std::cout << std::endl;
  try {
    adc.stopStream();
  }
  catch (RtAudioError& e) {
    e.printMessage();
  }
  if (adc.isStreamOpen()) {
    adc.closeStream();
  }
  file.close();
  start_flag = false;
  return true;
}













  play::play(size_t device_, size_t n_channels_, unsigned int buffer_size_, size_t first_channel_, size_t sample_rate_) {
    status = true;
    current_butch = 0;
    set_config(device_, n_channels_, buffer_size_, first_channel_, sample_rate_);
  }

  void play::set_config(size_t device_, size_t n_channels_, unsigned int buffer_size_,
                                        size_t first_channel_, size_t sample_rate_) {
    if (!device_) {
      parameters.deviceId = dac.getDefaultOutputDevice();
    } else {
      parameters.deviceId = device_;
    }
    sample_rate = sample_rate_;
    parameters.nChannels = n_channels_;
    parameters.firstChannel = first_channel_;
    buffer_size = buffer_size_;
  }

  void play::set_file(std::string puth) {
    file.open(puth, std::ios::binary | std::ios::ate);
    current_butch = 0;
    size_t n_bytes = file.tellg();
    duration = (double)n_bytes / parameters.nChannels / sizeof(MY_TYPE) / sample_rate;
  }

  void play::set_time(double percentage) {
    current_butch = duration * percentage * sample_rate  / buffer_size;
  }

  double play::current_time() {
    return (double)current_butch * buffer_size * parameters.nChannels / sample_rate;
  }

  void play::off() {status = false;}

  bool play::play_file(){
    status = true;
    file.seekg(current_butch * sizeof(MY_TYPE) * parameters.nChannels * buffer_size, std::ios_base::beg);
    try {
      dac.openStream(&parameters, nullptr,  FORMAT,
                      sample_rate, &buffer_size, &play_butch, nullptr);
      dac.startStream();
    }
    catch ( RtAudioError& e ) {
      e.printMessage();
      exit( 0 );
    }
    while (status && dac.isStreamRunning()) {
      // stream_time = dac.getStreamTime();
    }
    if ( dac.isStreamOpen() ) dac.closeStream();
    file.close();
    status = false;
    return true;
  }

  int play::play_butch(void *OutputBuffer,  void */*InputBuffer*/, unsigned int nBufferFrames,
                    double streamTime, RtAudioStreamStatus /*status1*/, void */*userData*/) {
      file.read((char*)OutputBuffer, sizeof(MY_TYPE) * parameters.nChannels * nBufferFrames);
      if(file.eof()){
        return 1;
      }
      current_butch++;
      return 0;
  }
}

