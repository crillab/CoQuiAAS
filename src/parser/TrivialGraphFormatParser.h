/**
 * \file TrivialGraphFormatParser.h
 * \brief Class used to parse trivial graph format input files
 * \author Jean-Marie Lagniez, Emmanuel Lonca, Jean-Guy Mailly
 * \version 0.1
 * \date 7/11/2014
 *
 */

#ifndef __TRIVIAL_GRAPH_FORMAT_PARSER_H__
#define __TRIVIAL_GRAPH_FORMAT_PARSER_H__

#include <iostream>


#include "IParser.h"
#include "Attacks.h"
#include "VarMap.h"

using namespace std;

/** 
 * \class TrivialGraphFormatParser
 * \brief Class used to parse TGF input files
 *
 */
class TrivialGraphFormatParser : public IParser {
  
 public:
  /**
   * \fn TrivialGraphFormatParser
   * \brief Constructor
   * \param input : the stream corresponding to the file to parse
   */
  TrivialGraphFormatParser(istream *input);

  /**
   * \fn ~TrivialGraphFormatParser()
   * \brief Destructor
   */
  virtual ~TrivialGraphFormatParser(){}

  void parseInstance();
  Attacks *getAttacks();
  VarMap *getVarMap();

 private:
  Attacks attacks;   /*!< The attack relation corresponding to the parsed file  */
  VarMap varMap; /**< The mapping between arguments names and literals */
  istream *is; /**< The stream to read the input AF */
};

#endif
