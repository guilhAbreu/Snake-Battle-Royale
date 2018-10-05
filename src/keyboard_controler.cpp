#include "../include/controler/keyboard_controler.hpp"

void error(char *msg);

void error(char *msg){
    perror(msg);
    exit(1);
}

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

  this->init_server(8888);
}

void Teclado::stop() {
  this->rodando = false;
  (this->kb_thread).join();
  close(this->socket_fd);
}

int Teclado::getchar() {
  int c = this->ultima_captura;
  this->ultima_captura = 0;
  return c;
}

void threadfun (int *keybuffer, int *control, int *socket_fd, int *connection_fd){
  int c = 0;
  int error = 0;
  socklen_t len = sizeof (error);
  while ((*control) == 1) {
    if(recv(*connection_fd, &c, sizeof(int), 0)){
      (*keybuffer) = c;
    }
    else if(getsockopt(*socket_fd, SOL_SOCKET, SO_ERROR, &error, &len)){
      (*keybuffer) = 27;
      *control = false;
    }
    else 
      (*keybuffer) = 0;
  }
  return;
}

bool Teclado::init_server(int port){

  this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  /*Create socket*/
  this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0) 
    error((char*)"ERROR opening socket");
  
  bzero((char *) &this->myself, sizeof(this->myself));/*set all values into the struct to 0*/
  this->portno = port; /*store port number*/
  
  this->myself.sin_family = AF_INET;
  this->myself.sin_addr.s_addr = INADDR_ANY; /*Store the IP Adress of the machine on which the server is running*/
  this->myself.sin_port = htons(this->portno);/*Convert portno into network bytes order*/
  
  if (bind(this->socket_fd, (struct sockaddr*)&this->myself, sizeof(this->myself)) != 0) {
    return -1;
  }

  listen(this->socket_fd, 2);

  connection_fd = accept(this->socket_fd, (struct sockaddr*)&client, &client_size);

  this->rodando = true;
  std::thread newthread(threadfun, &(this->ultima_captura), &(this->rodando), &this->socket_fd, &this->connection_fd);
  (this->kb_thread).swap(newthread);
  
  return 0;
}