#include <chrono>
#include <thread>

#include "../include/view/snake_view.hpp"
#include "../include/server/game_manager.hpp"

using namespace std::chrono;

int main (int argc, char *argv[]){
  if (argc < 4)
    error((char *)"IP address, PORT number and PLAYERS number must be passed\n");
  
  SNAKE_MAX = atoi(argv[3]);

  if (SNAKE_MAX < 2)
    error((char*)"IT IS NOT POSSIBLE OPEN A SERVER WITH ONLY ONE PLAYER\n");

  int portno = atoi(argv[2]), socket_fd;
  struct sockaddr_in myself, client;
  socklen_t client_size = (socklen_t)sizeof(client);
  
  init_server(portno, socket_fd, myself, argv[1]);
  
  // begin screen
  Tela *tela = new Tela(20, 20, 20, 20);
  tela->init();

  while(game_run(portno, socket_fd, myself, tela)){
    std::this_thread::sleep_for (std::chrono::milliseconds(5000));
    clear();refresh();
  }
  
  print_msg((int)LINES/2 + 1, -10 + (int)COLS/2, (char *)"Done!!!", false);
  std::this_thread::sleep_for (std::chrono::milliseconds(2000));

  shutdown(socket_fd, SHUT_RDWR);
  tela->stop();
  close(socket_fd);
  return 0;
}