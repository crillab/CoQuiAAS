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
  int fromVar = varMap.getVar(from);
  int toVar = varMap.getVar(to);
  attacks[toVar].push_back(fromVar);
  ++nbAttacks;
  if (attacks[toVar].size() > nMaxAttacks) nMaxAttacks = attacks[toVar].size();
  if(!from.compare(to))
    varMap.setSelfAttacking(from,true);
}

std::vector<int> *Attacks::getAttacksTo(int var) {
  return &attacks[var];
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

std::vector<std::tuple<bool, int, int, bool> >& Attacks::getDynAttacks() {
  return this->dynAttacks;
}

void Attacks::addDynAttack(bool add, std::string from, std::string to) {
  int fromVar = varMap.getVar(from);
  int toVar = varMap.getVar(to);
  bool wasPresent = false;
  for(unsigned int i=0; i<attacks[toVar].size(); ++i) {
    if(attacks[toVar][i] == fromVar) {
      wasPresent = true;
      for(unsigned int j=0; j<this->dynAttacks.size(); ++j) {
        if(std::get<1>(this->dynAttacks[j]) == fromVar && std::get<2>(this->dynAttacks[j]) == toVar) {
          wasPresent = std::get<3>(this->dynAttacks[j]);
          break;
        }
      }
      break;
    }
  }
  this->dynAttacks.push_back(std::make_tuple(add, fromVar, toVar, wasPresent));
}
