#ifndef __STAT_MAP_FACTORY_H__
#define __STAT_MAP_FACTORY_H__


#include "DefaultStatMap.h"
#include "FakeStatMap.h"
#include "LogStatMap.h"


using namespace std;

class StatMapFactory {

 public:

  static void createInstance(bool isFake) {

    // StatMapFactory::instance = (isFake) ? (StatMap*)(new FakeStatMap()) : (StatMap*)(new DefaultStatMap());
    StatMapFactory::instance = (isFake) ? (StatMap*)(new FakeStatMap()) : (StatMap*)(new LogStatMap(stdout));
  }

  static StatMap *getInstance() {

    return StatMapFactory::instance;
  }

  static void deleteInstance() {

    delete StatMapFactory::instance;
  }

 public:
  static StatMap *instance;
};

StatMap *StatMapFactory::instance = NULL;


#endif /* #ifndef __STAT_MAP_FACTORY_H__ */
