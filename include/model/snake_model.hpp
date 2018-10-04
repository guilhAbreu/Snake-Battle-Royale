#ifndef _SNAKE_MODEL_HPP
#define _SNAKE_MODEL_HPP

#include <iostream>
#include <vector>
#include "portaudio.h"

typedef struct Position{
  float x;
  float y;
}pos_2d;

typedef pos_2d vel_2d;

class Corpo {
  private:
  vel_2d velocidade;
  pos_2d posicao;

  public:
  Corpo(vel_2d velocidade, pos_2d p);
  void update(vel_2d nova_velocidade, pos_2d); // update velocity and position
  vel_2d get_velocidade(); // return velocity
  pos_2d get_posicao(); // return position
};

class Snake{
  private:
    std::vector<Corpo*> *corpos;

  public:
    Snake();
    void hard_copy(Snake *ldc); // create another body list (snake) and copy the original one
    void add_corpo(Corpo *c); // add a body
    std::vector<Corpo*> *get_corpos(); // return the list of bodys
};

class ListaDeSnakes {
  private:
    std::vector<Snake*> *snakes;

  public:
    ListaDeSnakes();
    void hard_copy(ListaDeSnakes *lds); // create another snake list and copy the original one
    void add_snake(Snake *c); // add one snake to the list
    std::vector<Snake*> *get_snakes(); // return the list of snakes
};

#endif
