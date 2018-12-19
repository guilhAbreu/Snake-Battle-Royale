#include <vector>

#include "../include/client/game_manager.hpp"

using namespace std::chrono;
using namespace client;

void init_asamples(std::vector<Audio::Sample*> *asamples); // init asamples
void rewind_samples(std::vector<Audio::Sample *> *asamples);

int main (int argc, char *argv[]){
  int portno;
  
  if (argc < 3) {
    fprintf(stderr,"usage %s IP_TARGET PORT\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  portno = atoi(argv[2]);
  
  // init asamples
  std::vector<Audio::Sample* > asamples(11);
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
  
  int ans = 0;
  do{
    button_player->volume = 0.6; soundboard_player->volume = 2.5;
    game_run(button_player, soundboard_player, asamples, tela, teclado, portno, argv[1]);
    button_player->volume = 0; soundboard_player->volume = 0;
    rewind_samples(&asamples);
    print_msg((int)LINES/2 + 0, -10 + (int)COLS/2, (char *)"Do you wanna play", true);
    print_msg((int)LINES/2 + 1, -15 + (int)COLS/2, (char *)"one more time on this server?", false);
    print_msg((int)LINES/2 + 2, -05 + (int)COLS/2, (char *)"[Y/n]", false);
    do{
      ans = teclado->getchar();
    }while(ans != 'Y' && ans != 'y' && ans != 'N' && ans != 'n');
  }while(ans == 'Y' || ans == 'y');

  // terminate objects properly
  button_player->stop();
  soundboard_player->stop();
  tela->stop();
  teclado->stop();
  return 0;
}

void rewind_samples(std::vector<Audio::Sample *> *asamples){
  for (int i = 0; i < asamples->size(); i++){
    (*asamples)[i]->set_position(0);
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
  return;
}