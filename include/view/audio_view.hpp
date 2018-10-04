#ifndef _AUDIO_HPP_
#define _AUDIO_HPP_

#include "../model/audio_model.hpp"
#include "portaudio.h"

namespace Audio{

class Player {
  private:
    Sample *audio_sample;
    bool playing;

    PaStreamParameters  outputParameters;
    PaStream*           stream;
    PaError             err;
    PaTime              streamOpened;


  public:
    Player();
    ~Player();
    float volume; // control volume
    void init(double sample_rate, unsigned int frames, float volume); // init player
    void pause(); // pause player
    void stop(); // stop player
    void play(Sample *audiosample); // associate a sample to the player
    Sample *get_data();
};

}

#endif
