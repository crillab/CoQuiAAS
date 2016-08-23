#ifndef __PARSER_FACTORY_H__
#define __PARSER_FACTORY_H__


#include "IParser.h"
#include "AspartixFormatParser.h"
#include "CNFFormatParser.h"
#include "TrivialGraphFormatParser.h"


/** 
 * \class ParserFactory
 * \brief Class used to get a parser instance and that provides static methods related to instance formats
 */
class ParserFactory {

 public:

  /**
   * \fn getInstanceFormat
   * \brief return an InstanceFormat object corresponding to the string passed in parameter
   * \param str : the string that may be related to an InstanceFormat object
   * \return an InstanceFormat object corresponding to the string passed in parameter ; FORMAT_UNDEFINED is returned in case the string is not recognized
   */
  static InstanceFormat getInstanceFormat(string str) {
    if(!str.compare("tgf")) return FORMAT_TRIVIAL_GRAPH;
    if(!str.compare("apx")) return FORMAT_ASPARTIX;
    if(!str.compare("cnf")) return FORMAT_CNF;
    return FORMAT_UNDEFINED;
  }

  /**
   * \fn getParserInstance
   * \brief return a parser object given an InstanceFormat object and a file
   * \param format : the instance format
   * \param file : a stream opened with read rights where pointer is at the beginning of the instance file
   * \return an InstanceFormat object corresponding to the string passed in parameter ; FORMAT_UNDEFINED is returned in case the string is not recognized
   */
  static IParser *getParserInstance(InstanceFormat format, istream *file) {
    switch(format) {
    case FORMAT_ASPARTIX:
      return new AspartixFormatParser(file);
    case FORMAT_CNF:
      return new CNFFormatParser(file);
    case FORMAT_TRIVIAL_GRAPH:
      return new TrivialGraphFormatParser(file);
    default: 
      return NULL;
    }
  }
  
};


#endif
