/**
 * \file AspartixFormatParser.cc
 * \brief Implementation of the class AspartixFormatParser
 * \author Jean-Marie Lagniez, Emmanuel Lonca, Jean-Guy Mailly
 * \version 0.1
 * \date 7/11/2014
 *
 */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cctype>
#include "AspartixFormatParser.h"

AspartixFormatParser::AspartixFormatParser(istream *input) : attacks(&varMap) {
  is = input;
}


static inline bool notOnlySpace(string line) {
  for(unsigned int i=0; i<line.size(); ++i)
    if(!isblank(line[i])) return true;
  return false;
}


void AspartixFormatParser::parseInstance() {

  if(*is){
    string line ;
    // Reading the args
    while(getline(*is,line)){
      if(!line.empty() && notOnlySpace(line)){
	if((line.find("att") == 0)){
	  // adding an attack
	  unsigned long commaIndex = line.find(",");
	  unsigned long closingParIndex = line.find(")");
	  if(commaIndex == string::npos || closingParIndex == string::npos){
	    cerr << "The line " << line << " contains an error (comma/closing paranthesis)" << endl ;
	    exit(-3) ;
	  }
	  string attacking = line.substr(4,commaIndex-4);
	  string attacked = line.substr(commaIndex+1,closingParIndex-commaIndex-1);
	  attacks.addAttack(attacking,attacked) ;
	}else{
	  // adding an argument if not already present
	  unsigned long closingParIndex = line.find(")");
	  if(closingParIndex == string::npos){
	    cerr << "The line " << line << " contains an error (closing paranthesis)" << endl ;
	    exit(-3) ;
	  }
	  string argName = line.substr(4, closingParIndex-4) ;
	  varMap.addEntry(argName);
	}
      }
    }
  }
}
    


inline Attacks *AspartixFormatParser::getAttacks() {
  return &attacks;
}


VarMap *AspartixFormatParser::getVarMap() {
  return &varMap;
}
