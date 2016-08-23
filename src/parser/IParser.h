/**
 * \file IParser.h
 * \brief Interface used to define the input files parsers
 * \author Jean-Marie Lagniez, Emmanuel Lonca, Jean-Guy Mailly
 * \version 0.1
 * \date 7/11/2014
 *
 */

#ifndef __IPARSER_H__
#define __IPARSER_H__

#include "Attacks.h"
#include "VarMap.h"

using namespace std;


/** enumeration of the attacks input formats*/
typedef enum {FORMAT_UNDEFINED, FORMAT_TRIVIAL_GRAPH, FORMAT_ASPARTIX, FORMAT_CNF} InstanceFormat;


/** 
 * \class IParser
 * \brief An interface for parser objects
 */
class IParser {
  
 public:
  /**
   * \fn parseInstance
   * \brief Parses the input file and build the data structures corresponding to the AF
   */
  virtual void parseInstance() = 0;

  /**
   * \fn getAttacks()
   * \brief Returns the attack relation corresponding to the input file
   * \return the attack relation corresponding to the input file
   */
  virtual Attacks *getAttacks() = 0;

  /**
   * \fn getVarMap()
   * \brief Returns the mapping between the arguments and the literals
   * \return the mapping between the arguments and the literals
   */
  virtual VarMap *getVarMap() = 0;

  virtual ~IParser(){}

};

#endif
