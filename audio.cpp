#include <iostream>
#include<queue>
#include <RtAudio.h>



template<typename T>
class audio
{
public:
	T get_to_transfer();
	void record_for_playback(T);
	virtual void stream() = 0;
protected:
	std::queue<T> in;
	std::queue<T> out;
private:
};

template <typename T>
class Rtaudio:public audio<T>
{
public:
Rtaudio() : channels(2), sampleRate(44100), bufferFrames(512), device(0) {}
using audio<T>::in;
using audio<T>::out;
virtual void stream();

private:
void record_for_transfer(T);
T get_to_playback();
int inout(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
		   double streamTime, RtAudioStreamStatus status, void *data )
size_t channels;
size_t sampleRate;
size_t bufferFrames;
size_t device;
};



int main() {

    return 0;
}