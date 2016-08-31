#include "DefaultStatMap.h"


using namespace CoQuiAAS;


DefaultStatMap::DefaultStatMap(){}


void DefaultStatMap::setStat(std::string name, std::string value) {
  
  stats[name] = value;
}


void DefaultStatMap::setStat(std::string name, int value) {
  std::ostringstream converter;

  converter << value;  
  stats[name] = converter.str();
}


void DefaultStatMap::setStat(std::string name, double value) {
  std::ostringstream converter;

  converter << value;  
  stats[name] = converter.str();
}


void DefaultStatMap::incCounterStat(std::string name, int value) {
  std::ostringstream converter;
  
  int newValue = value + (stats.find(name) == stats.end()) ? (0) : (atoi(stats[name].c_str()));
  converter << newValue;  
  stats[name] = converter.str();
}


void DefaultStatMap::printStats(FILE *outputFile) {

  std::map<std::string,std::string>::iterator it;
  for(it = stats.begin(); it != stats.end(); ++it) {
    fprintf(outputFile, "c %s: %s\n", it->first.c_str(), it->second.c_str());
  }
}

DefaultStatMap::~DefaultStatMap() {}
