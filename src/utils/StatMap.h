#ifndef __STAT_MAP_H__
#define __STAT_MAP_H__


#include <iostream>


using namespace std;


/**
 * \class StatMap
 * \brief Interface used to build a map storing statistics about problem resolution. The use of an interface is intended to provide both "real" and "fake" maps, that is one that effectively registers informations while the fake one does not do anything. These two statistics maps then allow to turn statistics on/off without any redundant if(...) blocks.
 */
class StatMap {

 public:
  /**
   * \fn setStat(string,string)
   * \brief add or update a new stat, given its name and its string value
   * \param name : the stat name
   * \param value : the stat value
   */
  virtual void setStat(string name, string value) = 0;

  /**
   * \fn setStat(string,int)
   * \brief add or update a new stat, given its name and its integer value
   * \param name : the stat name
   * \param value : the stat value
   */
  virtual void setStat(string name, int value) = 0;

  /**
   * \fn setStat(string,double)
   * \brief add or update a new stat, given its name and its floating value
   * \param name : the stat name
   * \param value : the stat value
   */
  virtual void setStat(string name, double value) = 0;

  /**
   * \fn incCounterStat(string,int)
   * \brief update a stat which value is an integer, given its name and the increment (default=1)
   * \param name : the stat name
   * \param value : the increment
   */
  virtual void incCounterStat(string name, int value=1) = 0;

  /**
   * \fn printStats(FILE*)
   * \brief write the statistics to an output file
   * \param outputFile : the file where the statistics must be written
   */
  virtual void printStats(FILE *outputFile) = 0;

  virtual ~StatMap(){};

};


#endif /* #ifndef __STAT_MAP_H__ */
