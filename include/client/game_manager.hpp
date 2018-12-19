#ifndef _GAME_MANAGER_
#define _GAME_MANAGER_

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

#include "../model/snake_model.hpp"
#include "../view/snake_view.hpp"
#include "../model/audio_model.hpp"
#include "../view/audio_view.hpp"
#include "../client/remote_keyboard.hpp"

using namespace client;

void error(char *msg);
void print_msg(int line, int collum, char *msg, bool clr);
void exit_msg(); // print exit message
void my_snake_color(short int color); // show snake color

int init_client(char *ip, int portno, int &socket_fd);
bool keyboard_map(int c, std::vector<Audio::Sample* > asamples, Audio::Player *button_player, \
                  Audio::Player *soundboard_player);
void soundboard_interaction(int food_counter, int bite_sognal, std::vector<Audio::Sample*> asamples,\
                  Audio::Player *soundboard_player); // define which soundboard to play
void threadscreen(char *keybuffer, bool *control, int socket_fd);
bool is_equal(std::vector<pos_2d> a, std::vector<pos_2d> b);
void game_run(Audio::Player *button_player,Audio::Player *soundboard_player, std::vector<Audio::Sample*> &asamples,\
              Tela *tela,Teclado *teclado, int portno, char *IP);

#endif