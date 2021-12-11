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




namespace audio {
    class record
    {
        public:
        record(size_t device, size_t n_channels, unsigned int buffer_size, 
                                size_t first_channel, size_t Sample_rate);
        bool input(std::string  puth);
        void off();
        void on();
        double stream_time();
        bool get_status() {return adc.isStreamRunning();};
        bool get_start_record() {return start_flag;}
        void set_config(size_t device, size_t n_channels, 
        unsigned int buffer_size, size_t first_channel, size_t Sample_rate);
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


    class play
    {
        public:
        play(size_t device_, size_t n_channels_, unsigned int buffer_size_, 
                                size_t first_channel_, size_t sample_rate_);
        void off();
        double current_time();
        void set_file(std::string puth);
        void set_time(double percentage);
        bool play_file();
        void set_config(size_t device_, size_t n_channels_, unsigned int buffer_size_, 
                                        size_t first_channel_, size_t sample_rate_);
        double get_duration() {return duration;}
        bool get_status() {return status;}

        private:
        static int play_butch(void *OutputBuffer,  void */*InputBuffer*/, unsigned int nBufferFrames,
                double /*streamTime*/, RtAudioStreamStatus /*status*/, void *userData );
        unsigned int buffer_size;
        size_t sample_rate;
        static inline RtAudio::StreamParameters parameters;
        double duration;
        static inline size_t current_butch;
        bool status;
        RtAudio dac;
        static inline std::ifstream file;
    };
}