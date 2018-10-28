#ifndef SERIALIZABLE_HPP
#define SERIALIZABLE_HPP

#include <fstream>
#include <iostream>
#include <string>
#include "../include/model/snake_model.hpp"

class RelevantData {
  private:
    std::vector<pos_2d> data;

  public:
    RelevantData();
    void PutData(std::vector<Corpo*> *corpos, short int color);
    void PutData(pos_2d p);
    void PutData(int food_counter);
    RelevantData(char *buffer_in);
    void serialize(char *buffer_out);
    int unserialize(char  *buffer_in);
    void copyData(std::vector<pos_2d> &dest);
    int get_data_size();
    void clean();
};

#endif
