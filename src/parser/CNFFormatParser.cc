/**
 * \file CNFFormatParser.cc
 * \brief Implementation of the class CNFFormatParser
 * \author Jean-Marie Lagniez, Emmanuel Lonca, Jean-Guy Mailly
 * \version 0.1
 * \date 7/11/2014
 *
 */

#include "CNFFormatParser.h"


using namespace CoQuiAAS;


CNFFormatParser::CNFFormatParser(std::istream *input) : attacks(varMap) {
  is = input;
}


void CNFFormatParser::parseInstance() {
  std::string word, secondWord;
  char buffer[1<<11];
  int nVars;

  *is >> word ;
  *is >> word ;
  *is >> nVars ;
  for(; nVars; --nVars) {
    snprintf(buffer,1<<11,"%d",nVars);
    varMap.addEntry(std::string(buffer));
  }
  *is >> word ;

  while(*is >> word) {
    *is >> secondWord ;
    attacks.addAttack(word, secondWord.substr(1,secondWord.length()-1));
    *is >> secondWord ;
  }
}

void CNFFormatParser::parseDynamics(std::istream *is) {
  std::cerr << "dynamics is not allowed for the CNF format" << std::endl;
  std::exit(1);
}


inline Attacks& CNFFormatParser::getAttacks() {
  return attacks;
}


inline VarMap& CNFFormatParser::getVarMap() {
  return varMap;
}
