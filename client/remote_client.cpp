#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <ncurses.h>
#include <string>
#include <stdlib.h>

#include "../include/model/snake_model.hpp"
#include "../include/view/snake_view.hpp"
#include "../include/player/remote_keyboard.hpp"
#include "../serial/serializable.hpp"

#include "../include/model/audio_model.hpp"
#include "../include/view/audio_view.hpp"

using namespace std::chrono;
using namespace client;

void print_msg(int line, int collum, char *msg, bool clr);
void exit_msg(); // print exit message
void my_snake_color(short int color); // show snake color

int init_client(char *ip, int portno, int &socket_fd);
void init_asamples(std::vector<Audio::Sample*> *asamples); // init asamples
bool keyboard_map(int c, std::vector<Audio::Sample* > asamples, Audio::Player *button_player, \
                  Audio::Player *soundboard_player);
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

  if (init_client(argv[1], portno, socket_fd) != 0)
    error((char *)"Unable to connect with the server\n");
  else{
    std::cout << "Connecttion attempt has been succeed" << std::endl;
  }

  // init asamples
  std::vector<Audio::Sample* > asamples(16);
  init_asamples(&asamples);

  // init players
  Audio::Player *button_player, *soundboard_player;
  button_player = new Audio::Player(), soundboard_player = new Audio::Player(); 
  button_player->init(44100, 64, 0.6), soundboard_player->init(44100, 256, 2.5);

  // ensure that button_player won't play at the beginning of the game
  asamples[1]->set_position(INT32_MAX);
  button_player->play(asamples[1]);
  
  // begin screen
  Tela *tela = new Tela(20, 20, 20, 20);
  tela->init();

  // begin keyboard interface
  Teclado *teclado = new Teclado();
  teclado->init();

  // Wait for OTHERS and catch your snake color
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  print_msg((int)LINES/2, -10 + (int)COLS/2, (char *)"WAITING FOR OTHERS...", true);

  short int my_color = -1;
  while(my_color <= 0 || my_color > 6)
    recv(socket_fd, &my_color, sizeof(short int), 0);

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  my_snake_color(my_color);
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  bool running = true;
  char buffer[2000000];
  RelevantData *data = new RelevantData();

  std::thread screen_thread(threadscreen, buffer, &running, socket_fd);

  // wait until a valid data arrive
  {
    pos_2d p = {10,-10};
    data->PutData(p);
    data->serialize(buffer);
    std::vector<pos_2d> recv_data;

    do{
      recv_data.clear();
      data->unserialize(buffer);
      data->copyData(recv_data);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }while(recv_data[1].x == 10 && recv_data[1].y == -10);
  }

  bool exit = false;
  int bite_signal = 0;
  while (!exit) {
    if (!running){
      // terminate objects properly
        running = false;
        screen_thread.join();
        button_player->stop();
        soundboard_player->stop();
        tela->stop();
        teclado->stop();
        close(socket_fd);
        print_msg(LINES/2, -10 + COLS/2, (char *)"YOU WERE DISCONNECTED...", true);
        return 0;
    }
    
    std::vector<pos_2d> recv_data;
    int food_counter = data->unserialize(buffer);
    data->copyData(recv_data);
    
    if (recv_data[1].x == -1 && (recv_data[1].y == -1 || recv_data[1].y == -2)){
      running = false;
      screen_thread.join();
      close(socket_fd);
      print_msg((int)LINES/2, -10 + (int)COLS/2, (char *)"GAME OVER", true);
      print_msg((int)LINES/2+1, -10 + (int)COLS/2, (char *)"YOU LOSE !!!", false);
      if (recv_data[1].y == -1)
        soundboard_player->play(asamples[3]);
      else
        soundboard_player->play(asamples[5]);
      std::this_thread::sleep_for(std::chrono::milliseconds(3000));
      break;
    }else if (recv_data[1].x == -10 && recv_data[1].y == 10){
      running = false;
      screen_thread.join();
      close(socket_fd);
      print_msg((int)LINES/2, -10 + (int)COLS/2, (char *)"YOU WON !!!", true);
      soundboard_player->play(asamples[0]);
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
      if (send(socket_fd, &c, sizeof(int), 0) == -1){
        // terminate objects properly
        running = false;
        screen_thread.join();
        button_player->stop();
        soundboard_player->stop();
        tela->stop();
        teclado->stop();
        close(socket_fd);
        print_msg(LINES/2, -10 + COLS/2, (char *)"YOU WERE DISCONNECTED...", true);
        return 0;
      }

      if (keyboard_map(c, asamples, button_player, soundboard_player)){
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
  button_player->stop();
  soundboard_player->stop();
  tela->stop();
  teclado->stop();
  return 0;
}

void threadscreen(char *keybuffer, bool *control, int socket_fd){
  int len_data = 1000, esc_value = 27;
  int bytes_recv;
  while ((*control) == true) {
    bytes_recv = recv(socket_fd, keybuffer, len_data, 0);
    if ((len_data - bytes_recv < len_data/2))
      len_data *=2;
    if (bytes_recv == 0){
      send(socket_fd, &esc_value, sizeof(int), 0);
      *control = false;
    }
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

  return connect(socket_fd, (struct sockaddr*)&target, sizeof(target));
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
                  Audio::Player *soundboard_player){
  switch (c){
    case KEY_DOWN:
    case KEY_LEFT:
    case KEY_UP:
    case KEY_RIGHT:
      asamples[1]->set_position(0);
      break;
    case 'm':
    case 'M':
      // turn on/off audio
      soundboard_player->volume = !soundboard_player->volume;
      button_player->volume = !button_player->volume;
      break;
    case 27:
      // terminate game
      soundboard_player->play(asamples[4]);
      return true;
  }
  return false;
}

void soundboard_interaction(int food_counter, int bite_signal, std::vector<Audio::Sample*> asamples, Audio::Player *soundboard_player){
  if (food_counter == 10){
    soundboard_player->play(asamples[9]);
  }
  else if (food_counter == 20){
    soundboard_player->play(asamples[10]);
  }
  else if (food_counter == 40){
    soundboard_player->play(asamples[8]);
  }
  else if(food_counter == 60){
    soundboard_player->play(asamples[0]);
  }
  else if (bite_signal){
    soundboard_player->play(asamples[2]);
    asamples[2]->set_position(0);
  }
  else if (food_counter == 0){
    soundboard_player->play(asamples[6]);
  }
  return;
}


void init_asamples(std::vector<Audio::Sample*> *asamples){
  for (int i = 0; i < asamples->size(); i++){
    (*asamples)[i] = new Audio::Sample();
  }

  (*asamples)[0]->load("audio/assets/Ce_e_o_bichao.dat");
  (*asamples)[1]->load("audio/assets/blip.dat");
  (*asamples)[2]->load("audio/assets/bite.dat");
  (*asamples)[3]->load("audio/assets/naovaidar.dat");
  (*asamples)[4]->load("audio/assets/bobao.dat");
  (*asamples)[5]->load("audio/assets/chupaessamanga.dat");
  (*asamples)[6]->load("audio/assets/horadoshow.dat");
  (*asamples)[7]->load("audio/assets/fatality.dat");
  (*asamples)[8]->load("audio/assets/animality.dat");
  (*asamples)[9]->load("audio/assets/pao.dat");
  (*asamples)[10]->load("audio/assets/brutality.dat");
  //(*asamples)[11]->load("audio/assets/finish_her.dat");
  //(*asamples)[12]->load("audio/assets/mario_star.dat");
  return;
}