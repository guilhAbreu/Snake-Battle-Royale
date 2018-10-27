#include "serializable.hpp"

#include <string.h>
#include <cstring>

RelevantData::RelevantData() {
};

void RelevantData::PutData(std::vector<Corpo*> *corpos, short int color){
  if (this->data.empty()){
    pos_2d len = {2,0}, init = {1,0};
    this->data.push_back(len);

    pos_2d p = {(float)corpos->size(), (float)color};
    this->data.push_back(init);
    this->data.push_back(p);
    
    for (int i = 0; i < corpos->size(); i++){
      this->data.push_back((*corpos)[i]->get_posicao());
      this->data[0].x++;
    }
  }else{
    
    pos_2d p = {(float)corpos->size(), (float)color};
    this->data.push_back(p);
    this->data[0].x++; // increase vector length counter
    this->data[1].x++; // increase snake counter

    for (int i = 0; i < corpos->size(); i++){
      this->data.push_back((*corpos)[i]->get_posicao());
      this->data[0].x++; // increase vector length counter
    }
  }
}

void RelevantData::PutData(pos_2d p){
  if (this->data.empty()){
    pos_2d len = {2,0}, init = {0,1};
    this->data.push_back(len);

    this->data.push_back(init);
    this->data.push_back(p);

  }else{
    
    this->data[0].x++; // increase vector length counter
    
    this->data.push_back(p);
    this->data[1].y++; // increase food counter
  }
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
  int bytes = sizeof(pos_2d);
  
  pos_2d p;
  std::memcpy(&p, &buffer_in[0], bytes);
  int data_size = p.x;
  
  for (int j = 1; j <= data_size; j++){
    pos_2d p;
    std::memcpy(&p, &buffer_in[bytes*j], bytes);
    this->data.push_back(p);
  }

}

int RelevantData::get_data_size(){
  return this->data.size()*sizeof(pos_2d);
}