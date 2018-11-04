#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
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

#define FOOD_AMOUNT_AT_BEGIN 8

int SNAKE_MAX;
std::mutex player_key;

typedef struct player_info{
  int socket;
  int port;
  int snake_ID;
  struct sockaddr_in myself;
  Fisica *physic;
  ListaDeSnakes *snake_list;
  bool *thread_running;
  int *connection_fd;
}plyr_data;

using namespace std::chrono;

uint64_t get_now_ms();
bool keyboard_map(int c, int snake_ID, Fisica *f, int *impulse);
Snake *create_snake(unsigned int length, pos_2d p); // create snake with length bodys
int init_server(int portno, int &socket_fd, struct sockaddr_in &myself);
void error(char *msg);
void player_management(plyr_data args);

int main (int argc, char *argv[]){
  if (argc < 3)
    error((char *)"PORT NUMBER and PLAYERS NUMBER must be passed\n");
  
  SNAKE_MAX = atoi(argv[2]);

  int portno = atoi(argv[1]), socket_fd;
  struct sockaddr_in myself, client;
  socklen_t client_size = (socklen_t)sizeof(client);
  
  init_server(portno, socket_fd, myself);
  
  // begin screen
  Tela *tela = new Tela(20, 20, 20, 20);
  tela->init();

  ListaDeSnakes *snake_list = new ListaDeSnakes();
  pos_2d p = {0, (float)LINES - 1};
  for(int i = 0; i < SNAKE_MAX; i++){
    Snake *snake = create_snake(4, p);
    snake_list->add_snake(snake);
    p.y-=5;
    if (p.y < 0)
      error((char *)"SNAKE_MAX is too large\n");
  }
  Fisica *physic = new Fisica(snake_list);
  
  tela->catch_param(snake_list, &physic->food_vector);

  bool thread_running[SNAKE_MAX];
  int connection_fd[SNAKE_MAX];

  for (int i = 0; i < SNAKE_MAX; i++)
    thread_running[i] = false;

  std::vector<std::thread> connection_thread(SNAKE_MAX);

  plyr_data args;

  for (int i = 0; i < FOOD_AMOUNT_AT_BEGIN; i++){
    physic->feed_snake();
  }
    
  args.socket = socket_fd;
  args.port = portno;
  args.connection_fd = connection_fd;
  args.myself = myself;
  args.physic = physic;
  args.snake_list = snake_list;
  args.thread_running = thread_running;

  for (int i = 0; i < SNAKE_MAX; i++){
    args.snake_ID = i;
    std::thread new_thread(player_management, args);
    connection_thread[i].swap(new_thread);
  }

  // Wait for all players
  int j = 0;
  while(j < SNAKE_MAX){
    if (thread_running[j] == true)
      j++;
  }

  int snakes_in_game;
  do{
    //tela->update();
    snakes_in_game = 0;
    for (int i = 0; i < SNAKE_MAX; i++){
      if (thread_running[i])
        snakes_in_game++;
    }
    std::this_thread::sleep_for (std::chrono::milliseconds(100));
  }while(snakes_in_game > 0);

  for (int i = 0; i < SNAKE_MAX; i++){
    connection_thread[i].join();
  }

  int i = 0;
  while(i < SNAKE_MAX){
    if (recv(connection_fd[i], NULL, 1, 0) == 0)
      i++;
  }

  std::this_thread::sleep_for (std::chrono::milliseconds(2000));
  tela->stop();
  close(socket_fd);
  return 0;
}

void player_management(plyr_data args){
  int socket_fd = args.socket, port = args.port, *connection_fd = args.connection_fd;
  struct sockaddr_in myself = args.myself;
  int snake_ID = args.snake_ID;
  Fisica *physic = args.physic;
  ListaDeSnakes *snake_list = args.snake_list;
  bool *thread_running = args.thread_running;

  struct sockaddr_in client;
  socklen_t client_size = (socklen_t)sizeof(client);
  
  connection_fd[snake_ID] = accept(socket_fd, (struct sockaddr*)&client, &client_size);

  // begin keyboard interface
  Teclado *teclado = new Teclado();
  teclado->init();
  teclado->get_server(port, socket_fd, connection_fd[snake_ID], myself, client);
  
  int impulse = 0; // speed up snake
  int food_counter = 0;
  int interation = 0;
  
  std::vector<Snake *> *snake_vector = snake_list->get_snakes();
  RelevantData *data = new RelevantData();

  thread_running[snake_ID] = true;

  // Wait for all players
  int i = 0;
  while(i < SNAKE_MAX){
    if (thread_running[i] == true)
      i++;
  }

  std::this_thread::sleep_for (std::chrono::milliseconds(500));

  short int ID = snake_ID + 1;
  send(connection_fd[snake_ID], &ID, sizeof(short int), 0);
  
  ID = 0;
  while(ID != snake_ID+1)
    recv(connection_fd[snake_ID], &ID, sizeof(short int), 0);
  
  while (thread_running[snake_ID]) {
    char buffer[2000000];
    
    short int update_value = physic->update(snake_ID, thread_running);

    if (update_value == -4){ // Snake ate
      food_counter++;
      player_key.lock();
      physic->feed_snake();
      player_key.unlock();
    }
    if (update_value >= -2){ // Snake losed
      pos_2d end_signal = {-1,-1};

      data->PutData(end_signal);
      data->serialize(buffer);
      send(connection_fd[snake_ID], buffer, data->get_data_size(), 0);

      if(update_value >= 0){
        thread_running[update_value] = false;
        send(connection_fd[update_value], buffer, data->get_data_size(), 0);
      }

      thread_running[snake_ID] = false;
      break;
    }
    if (update_value == -5){ // Snake won
      pos_2d end_signal = {-10,10};

      data->PutData(end_signal);
      data->serialize(buffer);
      send(connection_fd[snake_ID], buffer, data->get_data_size(), 0);

      thread_running[snake_ID] = false;
      break;
    }
    
    player_key.lock();
    for(int i = 0; i < SNAKE_MAX; i++){
      if (thread_running[i])
        data->PutData((*snake_vector)[i]->get_corpos(), SNAKE1_PAIR + i);
    }
    for(int i = 0; i < physic->food_vector.size(); i++){
      data->PutData(physic->food_vector[i]);
    }
    player_key.unlock();

    data->PutData(food_counter);
    data->serialize(buffer);
    send(connection_fd[snake_ID], buffer, data->get_data_size(), 0);
    data->clean();

    // read keys from keyboard
    int c = teclado->getchar();
    if (c > 0){
      if (keyboard_map(c, snake_ID, physic, &impulse) == false){
        thread_running[snake_ID] = false;
      }
    }

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

  teclado->stop();
  return;
}

bool keyboard_map(int c, int snake_ID, Fisica *f, int *impulse){
  switch (c){
    case KEY_DOWN:
      // head goes down
      f->change_dir(0,snake_ID);
      break;
    case KEY_LEFT:
      // head goes left
      f->change_dir(1,snake_ID);
      break;
    case KEY_UP:
      // head goes up
      f->change_dir(2,snake_ID);
      break;
    case KEY_RIGHT:
      // head goes right
      f->change_dir(3,snake_ID);
      break;
    case ' ':
      // speed up snake
      if (!(*impulse))
        *impulse = 50;
      break;
    case 27:
      // terminate game
      return false;
  }
  return true;
}

uint64_t get_now_ms() {
  return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

Snake *create_snake(unsigned int length, pos_2d p){
  vel_2d v = {(float)VEL,0};

  Snake *snake = new Snake();
  for (int i =0; i < length; i++){
    Corpo *c = new Corpo(v, p);
    snake->add_corpo(c);
    p.x-=1;
  }

  return snake;
}

int init_server(int portno, int &socket_fd, struct sockaddr_in &myself){
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