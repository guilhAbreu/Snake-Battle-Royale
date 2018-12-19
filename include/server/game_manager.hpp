#ifndef _GAME_MANAGER_
#define _GAME_MANAGER_

#include "../controler/snake_controler.hpp"
#include "../view/snake_view.hpp"

#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>

/*Socket*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define FOOD_AMOUNT_AT_BEGIN 8

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

extern int SNAKE_MAX;

bool keyboard_map(int c, int snake_ID, Fisica *f, int *impulse);
Snake *create_snake(unsigned int length, pos_2d p); // create snake with length bodys
int init_server(int portno, int &socket_fd, struct sockaddr_in &myself);
void player_management(plyr_data args);
bool game_run(int portno, int socket_fd, struct sockaddr_in myself, Tela *tela);

void error(char *msg);
void print_msg(int line, int collum, char *msg, bool clr);

#endif