#ifndef _KEYBOARD_HPP_
#define _KEYBOARD_HPP_

#include <ncurses.h>
#include <thread>

/*Socket*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void threadfun (int *keybuffer, int *control, int *socket_fd, int *connection_fd);

class Teclado {
  private:
    int ultima_captura; // last capture
    int rodando; // running
    std::thread kb_thread; // keyboard thread
    int socket_fd, connection_fd, portno;
    struct sockaddr_in myself, client;
    socklen_t client_size = (socklen_t)sizeof(client);

  public:
    Teclado();
    ~Teclado();
    void stop(); // terminate thread that gets keys from keyboard 
    void init(); // init thread that gets keys from keyboard 
    int getchar(); // return key read 
    void get_server(int portno, int socket, int connection,\
                            struct sockaddr_in myself, struct sockaddr_in client);
};

#endif
