/**
 * \file VarMap.cc
 * \brief Implementation of the class VarMap
 * \author Jean-Marie Lagniez, Emmanuel Lonca, Jean-Guy Mailly
 * \version 0.1
 * \date 7/11/2014
 *
 */


#include "VarMap.h"


using namespace CoQuiAAS;


VarMap::VarMap() {
  nvars = 0;
  nbSelfAttacking = 0;
}

void VarMap::addEntry(std::string name) {
  if(!contains(name)){
    varToName[++nvars] = name;
    selfAttacking[nvars] = false ;
    nameToVar[name] = nvars;
  }
}


std::string VarMap::getName(int var) {
  return varToName[var>0 ? var : -var];
}

int VarMap::getVar(std::string name) {
  int res = nameToVar[name];
  if(!res){
    addEntry(name);
    res = nvars;
  }
  return res;
}

bool VarMap::contains(std::string name){
  return (bool)nameToVar[name];
}

std::vector<int>* VarMap::intVars() {
  std::vector<int> *res = new std::vector<int>;
  for(std::map<int, std::string>::iterator it = varToName.begin(); it != varToName.end(); ++it)
    res->push_back(it->first);
  return res;
}

long VarMap::nVars() {
  return varToName.size();
}

bool VarMap::isSelfAttacking(int var){
  return selfAttacking[var];
}

bool VarMap::isSelfAttacking(std::string name){
  return isSelfAttacking(nameToVar[name]);
}

void VarMap::setSelfAttacking(int var, bool b){
  if(!selfAttacking[var]) ++nbSelfAttacking;
  selfAttacking[var] = b ;
}

void VarMap::setSelfAttacking(std::string name, bool b){
  setSelfAttacking(nameToVar[name], b);
}

unsigned int VarMap::nSelfAttacking(){
  return nbSelfAttacking;
}
