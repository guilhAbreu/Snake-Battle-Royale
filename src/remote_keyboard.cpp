#include "../include/client/remote_keyboard.hpp"

using namespace client;

void threadfun (int *keybuffer, bool *control)
{
  int c;
  while ((*control)) {
    c = getch();
    if (c!=ERR) (*keybuffer) = c;
  }
  return;
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

  this->rodando = true;
  std::thread newthread(threadfun, &(this->ultima_captura), &(this->rodando));
  (this->kb_thread).swap(newthread);
}

void Teclado::init(bool kpad_init) {
  if (kpad_init){
    // Inicializa ncurses
    raw();				         /* Line buffering disabled	*/
    keypad(stdscr, TRUE);	 /* We get F1, F2 etc..		*/
    noecho();			         /* Don't echo() while we do getch */
    curs_set(0);           /* Do not display cursor */
  }

  this->rodando = true;
  std::thread newthread(threadfun, &(this->ultima_captura), &(this->rodando));
  (this->kb_thread).swap(newthread);
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