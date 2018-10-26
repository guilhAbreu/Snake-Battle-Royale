#include "serializable.hpp"

#include <string.h>
#include <cstring>

RelevantData::RelevantData() {
};

void RelevantData::PutData(std::vector<Corpo*> *corpos){
  pos_2d p = {0,-2};
  for (int i = 0; i < corpos->size(); i++) {
    this->data.push_back((*corpos)[i]->get_posicao());
  }
  this->data.push_back(p);
}

void RelevantData::PutData(pos_2d p){
  this->data.push_back(p);
}

RelevantData::RelevantData(char *buffer_in) {
  this->unserialize(buffer_in);
}

void RelevantData::copyData(std::vector<pos_2d> &dest){
  for (int i = 0; i < this->data.size(); i++){
    dest.push_back(this->data[i]);
  }
}

void RelevantData::clean(){
  this->data.clear();
}

void RelevantData::serialize(char *buffer_out) {
  int bytes = sizeof(pos_2d);
  for (int i =0; i < this->data.size(); i++){
    pos_2d p = this->data[i];
    std::memcpy(&buffer_out[bytes*i], &p, bytes);
  }
}

void RelevantData::unserialize(char *buffer_in) {
  int i = -1, bytes = sizeof(pos_2d);
  do{
    i++;
    pos_2d p;
    std::memcpy(&p, &buffer_in[bytes*i], bytes);
    this->data.push_back(p);
  }while(i< 6);
}