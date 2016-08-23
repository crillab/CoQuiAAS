#ifndef __DEFAULT_STAT_MAP_H__
#define __DEFAULT_STAT_MAP_H__


#include <cstdio>
#include <cstdlib>
#include <map>
#include <sstream>

#include "StatMap.h"


using namespace std;


class DefaultStatMap : public StatMap {
  
 public:
  DefaultStatMap();

  /**
   * \fn setStat(string,string)
   * \brief add or update a new stat, given its name and its string value
   * \param name : the stat name
   * \param value : the stat value
   */
  void setStat(string name, string value);

  /**
   * \fn setStat(string,int)
   * \brief add or update a new stat, given its name and its integer value
   * \param name : the stat name
   * \param value : the stat value
   */
  void setStat(string name, int value);

  /**
   * \fn setStat(string,double)
   * \brief add or update a new stat, given its name and its floating value
   * \param name : the stat name
   * \param value : the stat value
   */
  void setStat(string name, double value);

  /**
   * \fn incCounterStat(string,int)
   * \brief update a stat which value is an integer, given its name and the increment (default=1)
   * \param name : the stat name
   * \param value : the increment
   */
  void incCounterStat(string name, int value=1);

  /**
   * \fn printStats(FILE*)
   * \brief write the statistics to an output file
   * \param outputFile : the file where the statistics must be written
   */
  void printStats(FILE *outputFile);

  virtual ~DefaultStatMap();

 private:
  map<string, string> stats;

};


#endif /* #define __DEFAULT_STAT_MAP_H__ */
