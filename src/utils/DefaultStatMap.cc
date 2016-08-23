#include "DefaultStatMap.h"


DefaultStatMap::DefaultStatMap(){}


void DefaultStatMap::setStat(string name, string value) {
  
  stats[name] = value;
}


void DefaultStatMap::setStat(string name, int value) {
  ostringstream converter;

  converter << value;  
  stats[name] = converter.str();
}


void DefaultStatMap::setStat(string name, double value) {
  ostringstream converter;

  converter << value;  
  stats[name] = converter.str();
}


void DefaultStatMap::incCounterStat(string name, int value) {
  ostringstream converter;
  
  int newValue = value + (stats.find(name) == stats.end()) ? (0) : (atoi(stats[name].c_str()));
  converter << newValue;  
  stats[name] = converter.str();
}


void DefaultStatMap::printStats(FILE *outputFile) {

  map<string,string>::iterator it;
  for(it = stats.begin(); it != stats.end(); ++it) {
    fprintf(outputFile, "c %s: %s\n", it->first.c_str(), it->second.c_str());
  }
}

DefaultStatMap::~DefaultStatMap() {}
