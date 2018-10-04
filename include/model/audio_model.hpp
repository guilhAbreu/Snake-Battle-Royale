#ifndef _AUDIO_MODEL_HPP_
#define _AUDIO_MODEL_HPP_

#include <iostream>
#include <vector>
#include "portaudio.h"

namespace Audio{

class Sample {
  private:
    std::vector<float> data;
    unsigned int position;

  public:
    Sample();
    ~Sample();
    void load(const char *filename); // load file
    std::vector<float> get_data(); // return file datas
    unsigned int get_position(); // return position of the next data to be echoed
    void set_position(unsigned int pos); // update position of the next data to be echoed
    bool finished(); 

};

}

#endif
