/**
 * \file Attacks.cc
 * \brief The implementation of the class Attack
 * author Jean-Marie Lagniez, Emmanuel Lonca, Jean-Guy Mailly
 * \version 0.1
 * \date 7/11/2014
 */


#include "Attacks.h"


using namespace CoQuiAAS;


Attacks::Attacks(VarMap *vm) : varMap(*vm) {
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
