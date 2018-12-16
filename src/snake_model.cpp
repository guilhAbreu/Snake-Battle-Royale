#include "../include/model/snake_model.hpp"

Corpo::Corpo(vel_2d velocidade, pos_2d p) {
  this->velocidade = velocidade;
  this->posicao = p;
}

void Corpo::update(vel_2d nova_velocidade, pos_2d p) {
  this->velocidade = nova_velocidade;
  this->posicao = p;
}

vel_2d Corpo::get_velocidade() {
  return this->velocidade;
}

pos_2d Corpo::get_posicao() {
  return this->posicao;
}

Snake::Snake() {
  this->corpos = new std::vector<Corpo *>(0);
}

void Snake::hard_copy(Snake *ldc) {
  std::vector<Corpo *> *corpos = ldc->get_corpos();

  for (int k=0; k<corpos->size(); k++) {
    Corpo *c = new Corpo((*corpos)[k]->get_velocidade(),(*corpos)[k]->get_posicao());
    this->add_corpo(c);
  }
}

void Snake::add_corpo(Corpo *c) {
  (this->corpos)->push_back(c);
}

std::vector<Corpo*> *Snake::get_corpos() {
  return (this->corpos);
}

ListaDeSnakes::ListaDeSnakes() {
  this->snakes = new std::vector<Snake *>(0);
}

void ListaDeSnakes::hard_copy(ListaDeSnakes *lds) {
  std::vector<Snake *> *snakes = lds->get_snakes();

  for (int k=0; k<snakes->size(); k++) {
    Snake *s = new Snake();
    std::vector<Corpo*> *c = (*snakes)[k]->get_corpos();
    for (int i=0; i < c->size(); i++){
        s->add_corpo((*c)[i]);
    }
    this->add_snake(s);
  }
}

void ListaDeSnakes::add_snake(Snake *s) {
  (this->snakes)->push_back(s);
}

std::vector<Snake*> *ListaDeSnakes::get_snakes() {
  return (this->snakes);
}