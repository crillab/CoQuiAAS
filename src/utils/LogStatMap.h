#ifndef __LOG_STAT_MAP_H__
#define __LOG_STAT_MAP_H__


#include <cstdio>
#include <cstdlib>
#include <map>
#include <sstream>

#include "StatMap.h"


namespace CoQuiAAS {


/**
 * \class LogStatMap
 * \brief A StatMap implementation which only logs the stats passed as parameters. Thus, the stats are immediately written in an output file as they are received. Do NOT use this class if any stat may be updated.
 */
class LogStatMap : public StatMap {
  
 public:
  LogStatMap(FILE *outFile);

  /**
   * \fn setStat(std::string,std::string)
   * \brief write a new stat, given its name and its string value
   * \param name : the stat name
   * \param value : the stat value
   */
  void setStat(std::string name, std::string value);

  /**
   * \fn setStat(std::string,int)
   * \brief write a new stat, given its name and its integer value
   * \param name : the stat name
   * \param value : the stat value
   */
  void setStat(std::string name, int value);

  /**
   * \fn setStat(std::string,double)
   * \brief write a new stat, given its name and its floating value
   * \param name : the stat name
   * \param value : the stat value
   */
  void setStat(std::string name, double value);

  /**
   * \fn incCounterStat(std::string,int)
   * \brief do NOT use this method in LogStatMap (only used in other StatMap classes)
   * \param name : irrelevant
   * \param value : irrelevant
   */
  void incCounterStat(std::string name, int value=1);

  /**
   * \fn printStats(FILE*)
   * \brief Useless in LogStatMap (only used in other StatMap classes)
   * \param outputFile : irrelevant
   */
  void printStats(FILE *outputFile);

  virtual ~LogStatMap();

 private:
  FILE *outfile;

};


}


#endif /* #define __LOG_STAT_MAP_H__ */
