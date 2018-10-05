#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <ncurses.h>

int main(int argc, char *argv[]) {
  
  int socket_fd, portno;
  struct sockaddr_in target;

  if (argc < 3) {
    fprintf(stderr,"usage %s IP_TARGET port\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  portno = atoi(argv[2]);

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  bzero((char *) &target, sizeof(target));
  target.sin_family = AF_INET;
  target.sin_port = htons(portno);
  inet_aton(argv[1], &(target.sin_addr));

  if (connect(socket_fd, (struct sockaddr*)&target, sizeof(target)) != 0) {
    return 0;
  }
  /* Agora, meu socket funciona como um descritor de arquivo usual */
  
  initscr();			       /* Start curses mode 		*/
	raw();				         /* Line buffering disabled	*/
  curs_set(0);           /* Do not display cursor */
  keypad(stdscr, TRUE); /*enable to get not just standards keys*/
  int c;
  do{
    c = getch();
    send(socket_fd, &c, sizeof(int), 0);
    printw("%c ",c);
  }while(c!=27);

  std::this_thread::sleep_for (std::chrono::milliseconds(5000));
  endwin();
  close(socket_fd);
  return 0;
}