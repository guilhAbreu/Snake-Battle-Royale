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
    void PutData(std::vector<Corpo*> *corpos);
    void PutData(pos_2d p);
    RelevantData(char *buffer_in);
    void serialize(char *buffer_out);
    void unserialize(char  *buffer_in);
    void copyData(std::vector<pos_2d> &dest);
    void clean();
};

#endif
