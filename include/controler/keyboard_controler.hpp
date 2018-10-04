#ifndef _KEYBOARD_HPP_
#define _KEYBOARD_HPP_

#include <ncurses.h>
#include <thread>

void threadfun (int *keybuffer, int *control);

class Teclado {
  private:
    int ultima_captura; // last capture
    int rodando; // running

    std::thread kb_thread; // keyboard thread

  public:
    Teclado();
    ~Teclado();
    void stop(); // terminate thread that gets keys from keyboard 
    void init(); // init thread that gets keys from keyboard 
    int getchar(); // return key read 
};

#endif
