#ifndef _KEYBOARD_CLIENT_HPP_
#define _KEYBOARD_CLIENT_HPP_

#include <ncurses.h>
#include <thread>

void threadfun(int *keybuffer, bool *control);

namespace client{

class Teclado {
  private:
    int ultima_captura; // last capture
    bool rodando; // running

    std::thread kb_thread; // keyboard thread

  public:
    Teclado();
    ~Teclado();
    void stop(); // terminate thread that gets keys from keyboard 
    void init(); // init thread that gets keys from keyboard
    void init(bool kpad_init);
    int getchar(); // return key read 
};
}
#endif