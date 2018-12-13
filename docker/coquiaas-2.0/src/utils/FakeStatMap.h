#ifndef __FAKE_STAT_MAP_H__
#define __FAKE_STAT_MAP_H__


#include "StatMap.h"


namespace CoQuiAAS {


class FakeStatMap : public StatMap {
  
 public:
  FakeStatMap();

  /**
   * \fn setStat(std::string,std::string)
   * \brief add or update a new stat, given its name and its string value
   * \param name : the stat name
   * \param value : the stat value
   */
  void setStat(std::string name, std::string value);

  /**
   * \fn setStat(std::string,int)
   * \brief add or update a new stat, given its name and its integer value
   * \param name : the stat name
   * \param value : the stat value
   */
  void setStat(std::string name, int value);

  /**
   * \fn setStat(std::string,double)
   * \brief add or update a new stat, given its name and its floating value
   * \param name : the stat name
   * \param value : the stat value
   */
  void setStat(std::string name, double value);

  /**
   * \fn incCounterStat(std::string,int)
   * \brief update a stat which value is an integer, given its name and the increment (default=1)
   * \param name : the stat name
   * \param value : the increment
   */
  void incCounterStat(std::string name, int value=1);

  /**
   * \fn printStats(FILE*)
   * \brief write the statistics to an output file
   * \param outputFile : the file where the statistics must be written
   */
  void printStats(FILE *outputFile);

  virtual ~FakeStatMap();

};


}


#endif /* #define __FAKE_STAT_MAP_H__ */
