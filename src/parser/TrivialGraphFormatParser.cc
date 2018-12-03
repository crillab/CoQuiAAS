/**
 * \file TrivialGraphFormatParser.cc
 * \brief Implementation of the class TrivialGraphFormatParser
 * \author Jean-Marie Lagniez, Emmanuel Lonca, Jean-Guy Mailly
 * \version 0.1
 * \date 7/11/2014
 *
 */


#include "TrivialGraphFormatParser.h"


using namespace CoQuiAAS;


TrivialGraphFormatParser::TrivialGraphFormatParser(std::istream *input) : attacks(varMap) {
  is = input;
}


void TrivialGraphFormatParser::parseInstance() {
  std::string word, secondWord;

  while(*is >> word) {
    if(!word.compare("#")) break;
    varMap.addEntry(word);
  }
  while(*is >> word) {
    *is >> secondWord;
    attacks.addAttack(word, secondWord);
  }
}

void TrivialGraphFormatParser::parseDynamics(std::istream *is) {
  std::string word, secondWord;
  while(*is >> word) {
    *is >> secondWord;
    bool add;
    if(word[0] == '-') add = false;
    else if(word[0] == '+') add = true;
    else {
      std::cerr << "The dynamic attack " << word << " --> " << secondWord << " contains an error (no/wrong dynamics operator)" << std::endl;
	    exit(-3);
    }
    attacks.addDynAttack(add, word.substr(1), secondWord);
  }
}


inline Attacks& TrivialGraphFormatParser::getAttacks() {
  return attacks;
}


inline VarMap& TrivialGraphFormatParser::getVarMap() {
  return varMap;
}
