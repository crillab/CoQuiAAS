#include "FakeStatMap.h"


using namespace CoQuiAAS;


FakeStatMap::FakeStatMap(){}


void FakeStatMap::setStat(std::string name, std::string value) {}


void FakeStatMap::setStat(std::string name, int value) {}


void FakeStatMap::setStat(std::string name, double value){}


void FakeStatMap::incCounterStat(std::string name, int value) {}


void FakeStatMap::printStats(FILE *outputFile) {}


FakeStatMap::~FakeStatMap() {}
