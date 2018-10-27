#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <ncurses.h>

#include "../../include/model/snake_model.hpp"
#include "../../include/view/snake_view.hpp"
#include "../../include/controler/snake_controler.hpp"

#include "../../include/model/audio_model.hpp"
#include "../../include/view/audio_view.hpp"
#include "../../include/controler/keyboard_controler.hpp"

#include "../../serial/serializable.hpp"

#include <string>
#include <cstring>

using namespace std::chrono;

uint64_t get_now_ms();
bool keyboard_map(int c, Fisica *f, int *impulse);
Snake *create_snake(unsigned int length); // create snake with length bodys
int init_server(int portno, int &socket_fd, int &connection_fd,\
                struct sockaddr_in &myself, struct sockaddr_in &client);
void error(char *msg);

int main (int argc, char *argv[]){
  if (argc != 2)
    error((char *)"PORT NUMBER must be passed\n");
  
  int portno = atoi(argv[1]), socket_fd, connection_fd;
  struct sockaddr_in myself, client;
  socklen_t client_size = (socklen_t)sizeof(client);
  
  init_server(portno, socket_fd, connection_fd, myself, client);
  
  Snake *snake = create_snake(4);
  
  // add snake into snake list and associate a physical model to it
  ListaDeSnakes *l = new ListaDeSnakes();
  l->add_snake(snake);
  Fisica *f = new Fisica(l);

  // begin screen
  Tela *tela = new Tela(l, &f->food_pos, 20, 20, 20, 20);
  tela->init();
  
  connection_fd = accept(socket_fd, (struct sockaddr*)&client, &client_size);

  // begin keyboard interface
  Teclado *teclado = new Teclado();
  teclado->init();
  teclado->get_server(portno, socket_fd, connection_fd, myself, client);
  
  int impulse = 0; // speed up snake
  int deltaT =1; // lock delta time in 1, in order to guarantee a discrete variation
  int food_counter = -1;
  bool exit = false;
  int interation = 0;
  
  RelevantData *data = new RelevantData();
  while (!exit) {

    // food_pos == (-1, don't care) means that there is no food at the arena 
    if (f->food_pos.x == -1){
      food_counter++;
      f->feed_snake();
    }

    // update model
    if(f->update(deltaT)) {
      char buffer[1000];
      pos_2d end_signal = {-1,-1};

      data->PutData(end_signal);
      data->serialize(buffer);
      send(connection_fd, buffer, data->get_data_size(), 0);
      break;
    }

    char buffer[1000];
    data->PutData(snake->get_corpos(), SNAKE1_PAIR);
    data->PutData(f->food_pos);
    data->serialize(buffer);
    send(connection_fd, buffer, data->get_data_size(), 0);
    data->clean();

    // read keys from keyboard
    int c = teclado->getchar();
    if (c >0)
      exit = keyboard_map(c, f, &impulse);

    if (interation > 40)
      impulse = 0;

    std::this_thread::sleep_for (std::chrono::milliseconds(100 - impulse));
    
    if (impulse || interation){
      if (interation == 40)
        impulse = 0;
      else if (interation == 50)
        interation = -1;
      interation++;
    }
  }

  // terminate objects properly
  tela->stop();
  teclado->stop();
  close(socket_fd);
  return 0;
}

bool keyboard_map(int c, Fisica *f, int *impulse){
  switch (c){
    case KEY_DOWN:
      // head goes down
      f->change_dir(0,0);
      break;
    case KEY_LEFT:
      // head goes left
      f->change_dir(1,0);
      break;
    case KEY_UP:
      // head goes up
      f->change_dir(2,0);
      break;
    case KEY_RIGHT:
      // head goes right
      f->change_dir(3,0);
      break;
    case ' ':
      // speed up snake
      if (!(*impulse))
        *impulse = 50;
      break;
    case 27:
      // terminate game
      return true;
  }
  return false;
}

uint64_t get_now_ms() {
  return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

Snake *create_snake(unsigned int length){
  pos_2d p = {40,40};
  vel_2d v = {(float)VEL,0};

  Snake *snake = new Snake();
  for (int i =0; i < length; i++){
    Corpo *c = new Corpo(v, p);
    snake->add_corpo(c);
    p.x-=1;
  }

  return snake;
}

int init_server(int portno, int &socket_fd, int &connection_fd,\
                struct sockaddr_in &myself, struct sockaddr_in &client){
  socklen_t client_size = (socklen_t)sizeof(client);
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  /*Create socket*/
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0) 
    error((char*)"ERROR opening socket");
  
  bzero((char *) &myself, sizeof(myself));/*set all values into the struct to 0*/
  
  myself.sin_family = AF_INET;
  myself.sin_addr.s_addr = INADDR_ANY; /*Store the IP Adress of the machine on which the server is running*/
  myself.sin_port = htons(portno);/*Convert portno into network bytes order*/
  
  if (bind(socket_fd, (struct sockaddr*)&myself, sizeof(myself)) != 0) {
    return -1;
  }

  listen(socket_fd, 2);
  return 0;
}

void error(char *msg){
    perror(msg);
    exit(1);
}