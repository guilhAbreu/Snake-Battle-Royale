/*
Nome: Guilherme de Brito Abreu
RA: 173691
*/

#include "../../include/model/snake_model.hpp"
#include "../../include/controler/snake_controler.hpp"
#include <ncurses.h>

#include "../../serial/serializable.hpp"

Fisica::Fisica(ListaDeSnakes *lds) {
  this->lista = lds;

  // indicate that there is no food yet
  this->food_pos.x =-1.0; food_pos.y = -1.0;
}

void Fisica::feed_snake(){
  srand(time(NULL));
  
  int y = rand()%LINES;
  int x = rand()%COLS;
  int j = 0 ;
  bool try_again =0;
  std::vector<Snake*> *s = this->lista->get_snakes();

  while(j < s->size()){
    std::vector<Corpo*> *corpos = (*s)[j]->get_corpos();
    j++;

    std::vector<pos_2d> last_pos(corpos->size());

    // get last positions
    for (int i = 0; i < corpos->size(); i++) {
      last_pos[i]= (*corpos)[i]->get_posicao();
    }

    // verify if already exist some body in the position x,y
    for (int i = 0; i < corpos->size(); i++){
      if ((int)last_pos[i].x == x && (int)last_pos[i].y == y){
        try_again = 1;
        break;
      }
    }

    if (try_again){
      try_again = 0;
      x = rand()%COLS;
      y = rand()%LINES;
      j = 0;
    }
  }

  this->food_pos.x = x, this->food_pos.y =y;
  return;
}

short int Fisica::update(int snake_ID) {
  // get snake list!
  std::vector<Snake*> *s = this->lista->get_snakes();

  std::vector<Corpo*> *c = (*s)[snake_ID]->get_corpos();
  vel_2d vel = (*c)[0]->get_velocidade();
  std::vector<pos_2d> last_pos(c->size());
  pos_2d new_pos;

  // get last positions
  for (int i = 0; i < c->size(); i++) {
    last_pos[i]= (*c)[i]->get_posicao();
  }
  
  // compute new position of the head
  new_pos.x = last_pos[0].x + vel.x;
  new_pos.y = last_pos[0].y + vel.y;

  // borders coditions
  if ((new_pos.x < 0) || (new_pos.y < 0)){
    return -2;
  }
  if ((new_pos.x >= COLS) || (new_pos.y >= LINES)){
    return -2;
  }

  // update snake position
  for (int i = 1; i < c->size(); i++) {
    (*c)[i]->update(vel, last_pos[i-1]);
  }
  (*c)[0]->update(vel, new_pos);
  
  // verify if snake got hited by itself
  short int collison = this->verify_snake_collison(s, snake_ID);
  if (collison != -3)
    return collison;

  // increase snake size or not
  if (this->verify_snake_ate(c)){
    Corpo *new_corpo = new Corpo(vel, last_pos[c->size()-1]);
    (*s)[snake_ID]->add_corpo(new_corpo);
    collison = -4;
  }

  return collison;
}

bool Fisica::verify_snake_ate(std::vector<Corpo*> *c){
  bool ate;

  if ((int)(*c)[0]->get_posicao().x == (int)this->food_pos.x \
        && (int)(*c)[0]->get_posicao().y == (int)this->food_pos.y){
      ate = true;
  }
  else{
    ate = false;
  }

  return ate;
}

short int Fisica::verify_snake_collison(std::vector<Snake*> *s, int snake_ID){
  std::vector<Corpo*> *snake_target = (*s)[snake_ID]->get_corpos();
  pos_2d head_pos = (*snake_target)[0]->get_posicao();
  for (int i = 0; i < s->size(); i++){
    if(i == snake_ID)
      continue;

    std::vector<Corpo*> *c = (*s)[i]->get_corpos();

    for(int j= 0; j < c->size(); j++){
      pos_2d p = (*c)[j]->get_posicao();
      if (head_pos.x == p.x && head_pos.y == p.y){
        s->erase(s->begin() + i);
        if (i > snake_ID)
          s->erase(s->begin() + snake_ID);
        else
          s->erase(s->begin() + snake_ID - 1);

        if (j == 0)
          return i;
        else
          return -1;
      }
    }
  }
  return -3;
}

void Fisica::change_dir(int direction, int i) {
  vel_2d new_vel;
  vel_2d last_vel;
  
  switch (direction){
    case 0:
      new_vel.x = 0;
      new_vel.y = VEL;
      break;
    case 1:
      new_vel.x = -VEL;
      new_vel.y = 0;
      break;
    case 2:
      new_vel.x = 0;
      new_vel.y = -VEL;  
      break;
    case 3:
      new_vel.x = VEL;
      new_vel.y = 0;
      break;
    default:
      return;
  }

  std::vector<Snake *> *s = this->lista->get_snakes();
  std::vector<Corpo *> *c = (*s)[i]->get_corpos();
  last_vel = (*c)[0]->get_velocidade();

  // do not turn around
  if ((new_vel.x < 0 && last_vel.x > 0) || (new_vel.x > 0 && last_vel.x < 0))
    return;
  else if ((new_vel.y < 0 && last_vel.y > 0) || (new_vel.y > 0 && last_vel.y < 0))
    return;
  else
    (*c)[0]->update(new_vel, (*c)[0]->get_posicao());
}