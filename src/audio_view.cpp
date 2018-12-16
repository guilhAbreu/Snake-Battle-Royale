#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "../include/view/audio_view.hpp"

using namespace Audio;

Player::Player() {
  this->playing = false;
  this->audio_sample = NULL;
}

void Player::pause() {
  this->playing = false;
}

Player::~Player() {

}

Sample *Player::get_data() {
  return this->audio_sample;
}


int mix_and_play (const void *inputBuffer, void *outputBuffer,
                  unsigned long framesPerBuffer,
                  const PaStreamCallbackTimeInfo* timeInfo,
                  PaStreamCallbackFlags statusFlags,
                  void *userData )

{
  Player *player = (Player*) userData;
  float *buffer = (float *) outputBuffer;
  Sample *s;
  s = player->get_data();
  if (s != NULL) {
    std::vector<float> data = s->get_data();
    unsigned int pos = s->get_position();

    // Fill the buffer with samples!
    for (int i=0; (i<framesPerBuffer); i++) {
      if (pos < data.size())
        buffer[i] = data[pos]*player->volume;
      else
        buffer[i] = 0;
      pos+=1;
    }
    s->set_position(pos);
  }
  else{
    // Fill the buffer with samples!
    for (int i=0; (i<framesPerBuffer); i++) {
      buffer[i] = 0;
    }  
  }
  return 0;
}

void Player::play(Sample *audiosample) {
  this->audio_sample = audiosample;
}

void Player::init(double sample_rate, unsigned int frames, float volume) {
  PaError err;

  err = Pa_Initialize();
  if( err != paNoError ) {
    std::cerr << "Error on Pa_Initialize()" << std::endl;
    return;
  }

  outputParameters.device = Pa_GetDefaultOutputDevice(); /* Default output device. */
  if (outputParameters.device == paNoDevice) {
    std::cerr << "Error: No default output device on Pa_GetDefaultOutputDevice()" << std::endl;
    return;
  }

  this->volume = volume;

  outputParameters.channelCount = 1;                     /* Mono output. */
  outputParameters.sampleFormat = paFloat32;
  outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;
  err = Pa_OpenStream( &stream,
                         NULL,      /* No input. */
                         &outputParameters,
                         sample_rate,
                         frames,       /* Frames per buffer. */
                         paClipOff, /* We won't output out of range samples so don't bother clipping them. */
                         mix_and_play,
                         this );

  if( err != paNoError ) {
    std::cerr << "Error on Pa_OpenStream()" << std::endl;
    return;
  }

  err = Pa_StartStream( stream );
  if( err != paNoError ) {
    std::cerr << "Error on Pa_StartStream()" << std::endl;
    return;
  }


}

void Player::stop() {
  PaError err;
  err = Pa_StopStream( stream );
  if( err != paNoError ) {
    std::cerr << "Error on Pa_StopStream()" << std::endl;
    return;
  }

  err = Pa_CloseStream( stream );
  if( err != paNoError ) {
    std::cerr << "Error on Pa_StopStream()" << std::endl;
    return;
  }

  Pa_Terminate();

}