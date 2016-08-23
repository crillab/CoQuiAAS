/**
 * \file TrivialGraphFormatParser.cc
 * \brief Implementation of the class TrivialGraphFormatParser
 * \author Jean-Marie Lagniez, Emmanuel Lonca, Jean-Guy Mailly
 * \version 0.1
 * \date 7/11/2014
 *
 */

#include "TrivialGraphFormatParser.h"


TrivialGraphFormatParser::TrivialGraphFormatParser(istream *input) : attacks(&varMap) {
  is = input;
}


void TrivialGraphFormatParser::parseInstance() {
  string word, secondWord;

  while(*is >> word) {
    if(!word.compare("#")) break;
    varMap.addEntry(word);
  }
  while(*is >> word) {
    *is >> secondWord;
    attacks.addAttack(word, secondWord);
  }
}


inline Attacks *TrivialGraphFormatParser::getAttacks() {
  return &attacks;
}


VarMap *TrivialGraphFormatParser::getVarMap() {
  return &varMap;
}
