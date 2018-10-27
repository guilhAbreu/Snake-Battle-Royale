#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <ncurses.h>
#include <string>

#include "../include/model/snake_model.hpp"
#include "../include/view/snake_view.hpp"
#include "../include/player/remote_keyboard.hpp"
#include "../serial/serializable.hpp"

using namespace std::chrono;
using namespace client;

void game_over_msg(); // print game over message
void exit_msg(); // print exit message
int init_client(char *ip, int portno, int &socket_fd);

void error(char *msg);
void threadscreen(char *keybuffer, bool *control, int socket_fd);

int main (int argc, char *argv[]){
  int socket_fd, portno;

  if (argc < 3) {
    fprintf(stderr,"usage %s IP_TARGET port\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  portno = atoi(argv[2]);

  init_client(argv[1], portno, socket_fd);
  std::cout << "Connection has been done" << std::endl;

  // begin screen
  Tela *tela = new Tela(20, 20, 20, 20);
  tela->init();

  // begin keyboard interface
  Teclado *teclado = new Teclado();
  teclado->init();

  RelevantData *data = new RelevantData();
  char buffer[2000000];

  bool rodando = true;
  std::thread screen_thread(threadscreen, buffer, &rodando, socket_fd);
  
  bool exit = false;
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  while (!exit) {
    
    std::vector<pos_2d> recv_data;
    data->unserialize(buffer);
    data->copyData(recv_data);
    if (recv_data[1].x == -1 && recv_data[1].y == -1){
      rodando = false;
      screen_thread.join();
      close(socket_fd);
      game_over_msg();
      std::this_thread::sleep_for(std::chrono::milliseconds(3000));
      break;
    }
    else
      tela->update(recv_data);
    data->clean();

    // read keys from keyboard
    int c = teclado->getchar();
    if (c > 0)
      send(socket_fd, &c, sizeof(int), 0);

    if (c == 27){
      exit = true;
      rodando = false;
      screen_thread.join();
      close(socket_fd);
      exit_msg();
    }
  }

  // terminate objects properly
  tela->stop();
  teclado->stop();
  return 0;
}

void threadscreen(char *keybuffer, bool *control, int socket_fd){
  int len_data = 1000;
  int bytes_recv;
  while ((*control) == true) {
    bytes_recv = recv(socket_fd, keybuffer, len_data, 0);
    if (len_data - bytes_recv < 100)
      len_data += 100;
    std::this_thread::sleep_for (std::chrono::milliseconds(10));
  }
  return;
}

int init_client(char *ip, int portno, int &socket_fd){
  struct sockaddr_in target;

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  std::cout << "socket created" << std::endl;


  bzero((char *) &target, sizeof(target));
  target.sin_family = AF_INET;
  target.sin_port = htons(portno);
  inet_aton(ip, &(target.sin_addr));

  std::cout << "trying to connect..." << std::endl;
  if (connect(socket_fd, (struct sockaddr*)&target, sizeof(target)) != 0) {
    return 0;
  }
}

void game_over_msg(){
  clear();
  attron(COLOR_PAIR(MSG_PAIR));
  move((int)LINES/2, -10 + (int)COLS/2);
  printw("GAME OVER");
  attroff(COLOR_PAIR(MSG_PAIR));
  refresh();
  return;
}

void exit_msg(){
  clear();
  attron(COLOR_PAIR(MSG_PAIR));
  move((int)LINES/2, -10 + (int)COLS/2);
  printw("BYE BYE");
  move((int)LINES/2 + 1, -10 + (int)COLS/2);
  printw("COME BACK SOON");
  attroff(COLOR_PAIR(MSG_PAIR));
  refresh();
  return;
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}