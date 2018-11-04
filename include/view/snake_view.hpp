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

/*Socket*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../model/snake_model.hpp"

#define SNAKE_SHAPE 'O'
#define FOOD_SHAPE 'o'

#define SNAKE1_PAIR 1
#define SNAKE2_PAIR 2
#define SNAKE3_PAIR 3
#define SNAKE4_PAIR 4

#define FOOD_PAIR 5
#define MSG_PAIR 6
#define BG_PAIR 7

class Tela {
  private:
    ListaDeSnakes *lista;
    std::vector<pos_2d> *food_pos;
    int maxI, maxJ;
    float maxX, maxY;
    int socket_fd, connection_fd, portno;
    struct sockaddr_in myself, client;
    socklen_t client_size = (socklen_t)sizeof(client);

  public:
    Tela(ListaDeSnakes *lds, std::vector<pos_2d> *food_pos, int maxI, int maxJ, float maxX, float maxY);
    Tela(int maxI, int maxJ, float maxX, float maxY);
    ~Tela();
    void catch_param(ListaDeSnakes *lds, std::vector<pos_2d> *food_pos);
    void stop(); // stop screen
    void init(); // init screen
    void update(); // update screen
    void update(std::vector<pos_2d> v);
    void get_server(int portno, int socket, int connection,\
                            struct sockaddr_in myself, struct sockaddr_in client);
};

#endif
