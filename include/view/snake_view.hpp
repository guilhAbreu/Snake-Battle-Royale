/*
Nome: Guilherme de Brito Abreu
RA: 173691
*/

#ifndef OO_VIEW_HPP
#define OO_VIEW_HPP

#include <ncurses.h>
#include <thread>
#include <vector>
#include <string>
#include <cstring>

#include "../model/snake_model.hpp"

#define SNAKE_SHAPE 'O'
#define FOOD_SHAPE 'o'

#define SNAKE1_PAIR 1
#define SNAKE2_PAIR 2
#define SNAKE3_PAIR 3
#define SNAKE4_PAIR 4
#define SNAKE5_PAIR 5
#define SNAKE6_PAIR 6

#define FOOD_PAIR 7
#define MSG_PAIR 8
#define BG_PAIR 9

class Tela {
  private:
    ListaDeSnakes *lista;
    std::vector<pos_2d> *food_pos;
    int maxI, maxJ;
    float maxX, maxY;

  public:
    Tela(ListaDeSnakes *lds, std::vector<pos_2d> *food_pos, int maxI, int maxJ, float maxX, float maxY);
    Tela(int maxI, int maxJ, float maxX, float maxY);
    ~Tela();
    void catch_param(ListaDeSnakes *lds, std::vector<pos_2d> *food_pos);
    void stop(); // stop screen
    void init(); // init screen
    void update(bool *ingame); // update screen
    void update(std::vector<pos_2d> v);
};

#endif
