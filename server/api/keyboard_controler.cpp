#include "../../include/controler/keyboard_controler.hpp"

Teclado::Teclado(){
}

Teclado::~Teclado() {
}

void Teclado::init() {
  // Inicializa ncurses
  raw();				         /* Line buffering disabled	*/
	keypad(stdscr, TRUE);	 /* We get F1, F2 etc..		*/
	noecho();			         /* Don't echo() while we do getch */
  curs_set(0);           /* Do not display cursor */
}

void Teclado::stop() {
  this->rodando = false;
  (this->kb_thread).join();
}

int Teclado::getchar() {
  int c = this->ultima_captura;
  this->ultima_captura = 0;
  return c;
}

void threadfun (int *keybuffer, int *control, int *socket_fd, int *connection_fd){
  while ((*control) == 1) {
    recv(*connection_fd, keybuffer, sizeof(int), 0);
    std::this_thread::sleep_for (std::chrono::milliseconds(10));
  }
  return;
}

void Teclado::get_server(int portno, int socket, int connection_fd,\
                          struct sockaddr_in myself, struct sockaddr_in client){

  this->socket_fd = socket;
  this->connection_fd = connection_fd;
  this->myself = myself;
  this->client = client;

  this->rodando = true;
  std::thread newthread(threadfun, &(this->ultima_captura), &(this->rodando), &this->socket_fd, &this->connection_fd);
  (this->kb_thread).swap(newthread);
}