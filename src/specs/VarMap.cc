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
  this->varToName.push_back("");
}

void VarMap::addEntry(std::string name) {
  if(!contains(name)){
	intVariables.push_back(++nvars);
    varToName.push_back(name);
    selfAttacking[nvars] = false ;
    nameToVar[name] = nvars;
  }
}


void VarMap::addEntry(std::vector<std::string> names) {
  intVariables.push_back(++nvars);
  std::stringstream sstream;
  for(unsigned int i=0; i<names.size(); ++i) {
    if(i>0) sstream << ",";
    std::string name = names[i];
    sstream << name;
    nameToVar[name] = nvars;
  }
  varToName.push_back(sstream.str());
}


std::string VarMap::getName(int var) {
  return varToName[var>0 ? var : -var];
}

std::vector<std::string> VarMap::getNames() {
  std::vector<std::string> names;
  for(std::map<std::string,int>::iterator it = nameToVar.begin(); it != nameToVar.end(); ++it) {
    names.push_back(it->first);
  }
  return names;
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

std::vector<int>& VarMap::intVars() {
  return intVariables;
}

long VarMap::nVars() {
  return varToName.size()-1;
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
