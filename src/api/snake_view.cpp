#include "../../include/model/snake_model.hpp"
#include "../../include/view/snake_view.hpp"
#include <stdlib.h>

Tela::Tela(ListaDeSnakes *lds, pos_2d *food_pos, int maxI, int maxJ, float maxX, float maxY) {
  this->food_pos = food_pos;
  this->lista = lds;
  this->maxI = maxI;
  this->maxJ = maxJ;
  this->maxX = maxX;
  this->maxY = maxY;
}

Tela::Tela(int maxI, int maxJ, float maxX, float maxY) {
  this->maxI = maxI;
  this->maxJ = maxJ;
  this->maxX = maxX;
  this->maxY = maxY;
}

void Tela::init() {
  initscr();			       /* Start curses mode 		*/
	raw();				         /* Line buffering disabled	*/
  curs_set(0);           /* Do not display cursor */
  keypad(stdscr, TRUE); /*enable to get not just standards keys*/

  /*init color mode on the screen*/
  start_color();
  init_pair(SNAKE1_PAIR, COLOR_RED, COLOR_WHITE);
  init_pair(SNAKE2_PAIR, COLOR_BLUE, COLOR_WHITE);
  init_pair(SNAKE3_PAIR, COLOR_MAGENTA, COLOR_WHITE);
  init_pair(SNAKE4_PAIR, COLOR_BLACK, COLOR_WHITE);

  init_pair(FOOD_PAIR, COLOR_GREEN, COLOR_WHITE);
  init_pair(MSG_PAIR, COLOR_BLUE, COLOR_YELLOW);
  init_pair(BG_PAIR, COLOR_WHITE, COLOR_WHITE);

  if (bkgd(COLOR_PAIR( BG_PAIR)) == !OK) exit(EXIT_FAILURE);
}

void Tela::get_server(int portno, int socket, int connection_fd,\
                          struct sockaddr_in myself, struct sockaddr_in client){

  this->socket_fd = socket;
  this->connection_fd = connection_fd;
  this->myself = myself;
  this->client = client;
}

void Tela::update() {
  std::vector<Snake*> *s = this->lista->get_snakes();

  //remove everything from the screen
  clear();

  
  for (int k =0; k < s->size(); k++){
    int i, j;
    std::vector<Corpo *> *corpos = (*s)[k]->get_corpos();

    // Draw bodys on the screen
    attron(COLOR_PAIR(SNAKE1_PAIR));
    for (int z = 0; z < corpos->size(); z++)
    {
      pos_2d p = (*corpos)[z]->get_posicao();
      i = (int)p.y * (this->maxI / this->maxX);
      j = (int)p.x * (this->maxI / this->maxX);
      mvaddch(i, j, SNAKE_SHAPE);
    }
    attroff(COLOR_PAIR(SNAKE1_PAIR));
  }

  // Draw food on the screen
  if (this->food_pos->x != -1){
    attron(COLOR_PAIR(FOOD_PAIR));
    mvaddch(this->food_pos->y, this->food_pos->x, FOOD_SHAPE);
    attroff(COLOR_PAIR(FOOD_PAIR));
  }
  
  // update scren
  refresh();
}

void Tela::update(std::vector<pos_2d> data){
  int num_snakes = data[0].x, num_foods = data[0].y;
  
  //remove everything from the screen
  clear();

  int k = 1;
  for(int i = 0; i < num_snakes; i++){
    int size_snake = data[k].x;
    int snake_pair = data[k].y;
    k++;

    // Draw snakes on the screen
    attron(COLOR_PAIR(snake_pair));
    for (int z = k; z < k + size_snake; z++){
      int i = (int)data[z].y * (this->maxI / this->maxX);
      int j = (int)data[z].x * (this->maxI / this->maxX);
      mvaddch(i, j, SNAKE_SHAPE);
    }
    attroff(COLOR_PAIR(snake_pair));
    k += size_snake;
  }
  
  // Draw foods on the screen
  attron(COLOR_PAIR(FOOD_PAIR));
  for(int z = k; z < k + num_foods; z++){

    int i = (int)data[z].y * (this->maxI / this->maxX);
    int j = (int)data[z].x * (this->maxI / this->maxX);
    mvaddch(i, j, FOOD_SHAPE);
  }
  attroff(COLOR_PAIR(FOOD_PAIR));

  // update scren
  refresh();
}

void Tela::stop() {
  endwin();
}

Tela::~Tela() {
  this->stop();;
}