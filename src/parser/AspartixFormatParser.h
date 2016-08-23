/**
 * \file AspartixFormatParser.h
 * \brief Class used to parse Aspartix format input files
 * \author Jean-Marie Lagniez, Emmanuel Lonca, Jean-Guy Mailly
 * \version 0.1
 * \date 7/11/2014
 *
 */

#ifndef __ASPARTIX_FORMAT_PARSER_H__
#define __ASPARTIX_FORMAT_PARSER_H__

#include <iostream>
#include <string>

#include "IParser.h"
#include "Attacks.h"
#include "VarMap.h"

using namespace std;

/** 
 * \class AspartixFormatParser
 * \brief Class used to parse Aspartix input files
 *
 */
class AspartixFormatParser : public IParser {
  
 public:
  /**
   * \fn AspartixFormatParser
   * \brief Constructor
   * \param input : the stream to parse
   */
  AspartixFormatParser(istream *input);

  /**
   * \fn ~AspartixFormatParser
   * \brief Destructor
   */
  virtual ~AspartixFormatParser(){}
  void parseInstance();
  Attacks *getAttacks();
  VarMap *getVarMap();

 private:
  Attacks attacks;  /*!< The attack relation corresponding to the parsed file  */
  VarMap varMap; /**< The mapping between arguments names and literals */
  istream *is; /**< The stream to read the input AF */
};

#endif
