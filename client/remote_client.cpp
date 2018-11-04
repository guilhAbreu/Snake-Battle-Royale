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
#include "../include/model/audio_model.hpp"
#include "../include/view/audio_view.hpp"

using namespace std::chrono;
using namespace client;

bool bg_enable;

void bg_music_msg(Teclado *teclado);
void print_msg(int line, int collum, char *msg, bool clr);
void exit_msg(); // print exit message
void my_snake_color(short int color); // show snake color

int init_client(char *ip, int portno, int &socket_fd);
void init_asamples(std::vector<Audio::Sample*> *asamples); // init asamples
bool keyboard_map(int c, std::vector<Audio::Sample* > asamples, Audio::Player *button_player, \
                  Audio::Player *soundboard_player, Audio::Player *background_player);
void soundboard_interaction(int food_counter, int bite_sognal, std::vector<Audio::Sample*> asamples,\
                  Audio::Player *soundboard_player); // define which soundboard to play

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

  bg_enable = false;

  // init asamples
  std::vector<Audio::Sample* > asamples(16);
  init_asamples(&asamples);

  // init players
  Audio::Player *button_player, *soundboard_player, *background_player;
  button_player = new Audio::Player(), soundboard_player = new Audio::Player(); 
  background_player = new Audio::Player();  
  button_player->init(44100, 64, 0.6), soundboard_player->init(44100, 256, 2.5);
  background_player->init(44100, 2048, 0.2);

  // ensure that button_player won't play at the beginning of the game
  asamples[3]->set_position(INT32_MAX);
  button_player->play(asamples[3]);
  
  // begin screen
  Tela *tela = new Tela(20, 20, 20, 20);
  tela->init();

  // begin keyboard interface
  Teclado *teclado = new Teclado();
  teclado->init();

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  print_msg((int)LINES/2, -10 + (int)COLS/2, (char *)"WAITING FOR OTHERS...", true);

  short int my_color = -1;
  while(my_color <= 0 || my_color > 4)
    recv(socket_fd, &my_color, sizeof(short int), 0);

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  my_snake_color(my_color);
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  send(socket_fd, &my_color, sizeof(short int), 0);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  bool running = true;
  char buffer[2000000];
  RelevantData *data = new RelevantData();

  std::thread screen_thread(threadscreen, buffer, &running, socket_fd);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  {
    pos_2d p = {10,-10};
    data->PutData(p);
    data->serialize(buffer);
    std::vector<pos_2d> recv_data;

    do{
      recv_data.clear();
      data->unserialize(buffer);
      data->copyData(recv_data);
    }while(recv_data[1].x == 10 && recv_data[1].y == -10);
  }

  bool exit = false;
  int bite_signal = 0;
  while (!exit) {

    if (bg_enable && asamples[1]->finished())
      asamples[1]->set_position(0);

    std::vector<pos_2d> recv_data;
    int food_counter = data->unserialize(buffer);
    data->copyData(recv_data);
    
    if (recv_data[1].x == -1 && recv_data[1].y == -1){
      running = false;
      screen_thread.join();
      close(socket_fd);
      print_msg((int)LINES/2, -10 + (int)COLS/2, (char *)"GAME OVER", true);
      print_msg((int)LINES/2+1, -10 + (int)COLS/2, (char *)"YOU LOSE !!!", false);
      soundboard_player->play(asamples[5]);
      std::this_thread::sleep_for(std::chrono::milliseconds(3000));
      break;
    }else if (recv_data[1].x == -10 && recv_data[1].y == 10){
      running = false;
      screen_thread.join();
      close(socket_fd);
      print_msg((int)LINES/2, -10 + (int)COLS/2, (char *)"YOU WON !!!", true);
      soundboard_player->play(asamples[2]);
      std::this_thread::sleep_for(std::chrono::milliseconds(6000));
      break;
    }else
      tela->update(recv_data);
    
    data->clean();

    bite_signal = food_counter -bite_signal;
    soundboard_interaction(food_counter, bite_signal, asamples, soundboard_player);
    bite_signal = food_counter;

    // read keys from keyboard
    int c = teclado->getchar();
    if (c > 0){
      send(socket_fd, &c, sizeof(int), 0);

      if (keyboard_map(c, asamples, button_player, soundboard_player, background_player)){
        exit = true;
        running = false;
        screen_thread.join();
        close(socket_fd);
        exit_msg();
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
      }
    }
  }

  // terminate objects properly
  background_player->stop();
  button_player->stop();
  soundboard_player->stop();
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

  bzero((char *) &target, sizeof(target));
  target.sin_family = AF_INET;
  target.sin_port = htons(portno);
  inet_aton(ip, &(target.sin_addr));

  if (connect(socket_fd, (struct sockaddr*)&target, sizeof(target)) != 0) {
    return 0;
  }
  else{
    return 1;
  }
}

void bg_music_msg(Teclado *teclado){
  bool ok = false;
  attron(COLOR_PAIR(MSG_PAIR));
  while (!ok){
    move((int)LINES/2, -10 + (int)COLS/2);
    printw("DO YOU WANNA LOAD BACKGROUND MUSIC? [Y/n]");
    move((int)LINES/2 + 1, -10 + (int)COLS/2);
    printw("KEEP IN MIND THAT BACKGROUND MUSIC IS VERY LARGE");
    move((int)LINES/2 + 2, -10 + (int)COLS/2);
    printw("AND IT CAN LAST TOO MUCH TIME TO LOAD IT.");
    refresh();
    int c = teclado->getchar();
    switch (c){
      case 'Y':
      case 'y':
        bg_enable = true;
        ok = true;
        break;
      case 'N':
      case 'n':
        bg_enable = false;
        ok = true;
        break;
    }
  }
  clear();
  move((int)LINES/2, -10 + (int)COLS/2);
  printw("LOADING ...") ;
  refresh();
  attroff(COLOR_PAIR(MSG_PAIR));
  return;
}

void my_snake_color(short int color){
  clear();
  attron(COLOR_PAIR(color));
  move((int)LINES/2, -10 + (int)COLS/2);
  printw("YOUR SNAKE:");
  move((int)LINES/2+2, -10 + (int)COLS/2);
  printw("OOOOOOOO");
  attroff(COLOR_PAIR(color));
  refresh();
  return;
}

void print_msg(int line, int collum, char *msg, bool clr){
  if (clr) clear();
  attron(COLOR_PAIR(MSG_PAIR));
  move(line, collum);
  printw(msg);
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

bool keyboard_map(int c, std::vector<Audio::Sample* > asamples, Audio::Player *button_player, \
                  Audio::Player *soundboard_player, Audio::Player *background_player){
  switch (c){
    case KEY_DOWN:
    case KEY_LEFT:
    case KEY_UP:
    case KEY_RIGHT:
      asamples[3]->set_position(0);
      break;
    case 'I':
    case 'i':
      if (!bg_enable) break;
      background_player->volume+=0.1;
      if (background_player->volume > 2)
        background_player->volume = 2;
      break;
    case 'D':
    case 'd':
      if (!bg_enable) break;
      background_player->volume-=0.1;
      if (background_player->volume <= 0)
        background_player->volume = 0;
      break;
    case 'm':
    case 'M':
      // turn on/off audio
      background_player->volume =  !background_player->volume;
      if (background_player->volume > 0) background_player->volume = 0.5;
      soundboard_player->volume = !soundboard_player->volume;
      button_player->volume = !button_player->volume;
      break;
    case 27:
      // terminate game
      soundboard_player->play(asamples[6]);
      return true;
  }
  return false;
}

void soundboard_interaction(int food_counter, int bite_signal, std::vector<Audio::Sample*> asamples, Audio::Player *soundboard_player){
  if (food_counter == 10){
    soundboard_player->play(asamples[14]);
  }
  else if (food_counter == 20){
    soundboard_player->play(asamples[12]);
  }
  else if (food_counter == 40){
    soundboard_player->play(asamples[11]);
  }
  else if(food_counter == 60){
    soundboard_player->play(asamples[2]);
  }
  else if (bite_signal){
    soundboard_player->play(asamples[4]);
    asamples[4]->set_position(0);
  }
  /*else if (food_counter == 0){
    soundboard_player->play(asamples[8]);
  }*/
  return;
}


void init_asamples(std::vector<Audio::Sample*> *asamples){
  for (int i = 0; i < asamples->size(); i++){
    (*asamples)[i] = new Audio::Sample();
  }
  /*
  the first position is reserved for background songs that were not chosen yet
  */
  if (bg_enable) (*asamples)[1]->load("audio/assets/sonic_theme.dat");
  (*asamples)[2]->load("audio/assets/Ce_e_o_bichao.dat");
  (*asamples)[3]->load("audio/assets/blip.dat");
  (*asamples)[4]->load("audio/assets/bite.dat");
  (*asamples)[5]->load("audio/assets/naovaidar.dat");
  (*asamples)[6]->load("audio/assets/get_over_here.dat");
  //(*asamples)[7]->load("audio/assets/come_here.dat");
  (*asamples)[8]->load("audio/assets/mortal_kombat_theme.dat");
  //(*asamples)[9]->load("audio/assets/finish_him.dat");
  //(*asamples)[10]->load("audio/assets/finish_her.dat");
  (*asamples)[11]->load("audio/assets/fatality.dat");
  (*asamples)[12]->load("audio/assets/animality.dat");
  //(*asamples)[13]->load("audio/assets/soul_suffer.dat");
  (*asamples)[14]->load("audio/assets/brutality.dat");
  //(*asamples)[15]->load("audio/assets/mario_star.dat");
  return;
}