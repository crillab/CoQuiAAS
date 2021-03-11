/**
 * \file Attacks.cc
 * \brief The implementation of the class Attack
 * author Jean-Marie Lagniez, Emmanuel Lonca, Jean-Guy Mailly
 * \version 0.1
 * \date 7/11/2014
 */


#include "Attacks.h"


using namespace CoQuiAAS;


Attacks::Attacks(VarMap& vm) : varMap(vm) {
  nbAttacks = 0;
  nMaxAttacks = 0;
}


void Attacks::addAttack(std::string from, std::string to) {
  std::vector<std::string>& attacksTo = attacks[to];
  attacksTo.push_back(from);
  ++nbAttacks;
  if (attacksTo.size() > nMaxAttacks) nMaxAttacks = attacksTo.size();
  if(!from.compare(to))
    varMap.setSelfAttacking(from,true);
}

std::vector<std::string>& Attacks::getAttacksTo(std::string var) {
  return this->attacks[var];
}

unsigned int Attacks::nAttacks() {
  return nbAttacks;
}

unsigned int Attacks::maxAttacks() {
  return nMaxAttacks;
}

VarMap &Attacks::getVarMap() {
	return varMap;
}

std::vector<std::tuple<bool, std::string, std::string, bool> >& Attacks::getDynAttacks() {
  return this->dynAttacks;
}

void Attacks::addDynAttack(bool add, std::string from, std::string to) {
  bool wasPresent = false;
  for(unsigned int i=0; i<attacks[to].size(); ++i) {
    if(attacks[to][i] == from) {
      wasPresent = true;
      for(unsigned int j=0; j<this->dynAttacks.size(); ++j) {
        if(std::get<1>(this->dynAttacks[j]) == from && std::get<2>(this->dynAttacks[j]) == to) {
          wasPresent = std::get<3>(this->dynAttacks[j]);
          break;
        }
      }
      break;
    }
  }
  if(!wasPresent) addAttack(from, to);
  this->dynAttacks.push_back(std::make_tuple(add, from, to, wasPresent));
}
