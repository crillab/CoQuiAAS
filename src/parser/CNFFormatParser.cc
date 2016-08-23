/**
 * \file CNFFormatParser.cc
 * \brief Implementation of the class CNFFormatParser
 * \author Jean-Marie Lagniez, Emmanuel Lonca, Jean-Guy Mailly
 * \version 0.1
 * \date 7/11/2014
 *
 */

#include "CNFFormatParser.h"


CNFFormatParser::CNFFormatParser(istream *input) : attacks(&varMap) {
  is = input;
}


void CNFFormatParser::parseInstance() {
  string word, secondWord;
  char buffer[1<<11];
  int nVars;

  *is >> word ;
  *is >> word ;
  *is >> nVars ;
  for(; nVars; --nVars) {
    snprintf(buffer,1<<11,"%d",nVars);
    varMap.addEntry(string(buffer));
  }
  *is >> word ;

  while(*is >> word) {
    *is >> secondWord ;
    attacks.addAttack(word, secondWord.substr(1,secondWord.length()-1));
    *is >> secondWord ;
  }
}


inline Attacks *CNFFormatParser::getAttacks() {
  return &attacks;
}


VarMap *CNFFormatParser::getVarMap() {
  return &varMap;
}
