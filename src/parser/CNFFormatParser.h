/**
 * \file CNFFormatParser.h
 * \brief Class used to parse CNF format input files
 * \author Jean-Marie Lagniez, Emmanuel Lonca, Jean-Guy Mailly
 * \version 0.1
 * \date 7/11/2014
 *
 */

#ifndef __CNF_FORMAT_PARSER_H__
#define __CNF_FORMAT_PARSER_H__

#include <iostream>
#include <cstdio>

#include "IParser.h"
#include "Attacks.h"
#include "VarMap.h"

using namespace std;

/** 
 * \class CNFFormatParser
 * \brief Class used to parse CNF input files
 *
 */
class CNFFormatParser : public IParser {
  
 public:
  /**
   * \fn CNFFormatParser
   * \brief Constructor
   * \param input : the stream corresponding to the file to parse
   */
  CNFFormatParser(istream *input);

  /**
   * \fn ~CNFFormatParser
   * \brief Destructor
   */
  virtual ~CNFFormatParser(){}

  void parseInstance();
  Attacks *getAttacks();
  VarMap *getVarMap();

 private:
  Attacks attacks; /*!< The attack relation corresponding to the parsed file  */
  VarMap varMap; /**< The mapping between arguments names and literals */
  istream *is; /**< The stream to read the input AF */
};

#endif
