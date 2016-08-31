#ifndef __COMMAND_LINE_HELPER_H__
#define __COMMAND_LINE_HELPER_H__

#include <iostream>
#include <vector>
#include <map>

#include "SolverFactory.h"
#include "ParserFactory.h"


namespace CoQuiAAS {


/** 
 * \class CommandLineHelper
 * \brief Class used to parse the command line variables provided to the main function
 */
class CommandLineHelper {

 public:
  /**
   * \fn CommandLineHelper
   * \brief Constructor
   * \param argc : the number of program's arguments passed in main function
   * \param argv : the program's arguments passed in main function
   */
  CommandLineHelper(int argc, char** argv);

  /**
   * \fn parseCommandLine
   * \brief Constructor
   */  
  void parseCommandLine();

  /**
   * \fn getSemanticName
   * \brief returns the semantic name passed in main arguments
   * \return a SemanticName object representing the semantic name
   */
  SemanticName getSemanticName();

  /**
   * \fn getTaskType
   * \brief returns the task type passed in main arguments
   * \return a TaskType object representing the task
   */
  TaskType getTaskType();

  /**
   * \fn getInstanceFormat
   * \brief returns the instance format passed in main arguments
   * \return an InstanceFormat object representing the instance format
   */
  InstanceFormat getInstanceFormat();

  /**
   * \fn getInstanceFile
   * \brief returns the instance file passed in main arguments
   * \return a string representing the instance file
   */
  string getInstanceFile();

  /**
   * \fn getAdditionalParameter
   * \brief returns the additional parameter, or an empty string if none
   * \return the additional parameter
   */
  string getAdditionalParameter(string param);

  /**
   * \fn getAdditionalParams()
   * \brief allows to know the whole set of additional parameters defined by the command line
   * \return the parameters' map
   */
  map<string,string> *getAdditionalParams();

  /**
   * \fn mustExitNow
   * \brief returns a boolean indicating if the solver must exist after parsing
   * \return a boolean indicating if the solver must exit after parsing ; this occurs with some solver options or if an error occurred
   */
  bool mustExitNow();

  /**
   * \fn errorInCommandLine
   * \brief return a boolean indicating if an error occurred while parsing the command line
   * \return a boolean indicating if an error occurred while parsing the command line ; this check may be done after mustExitNow() returned true
   */
  bool errorInCommandLine();

  /**
   * \fn outputProblemAndExit
   * \brief return a boolean indicated if the solver must display its constraints and exit ; the output format depends of the solver
   * \return a boolean indicated if the solver must display its constraints and exit
   */
  bool outputProblemAndExit();

 private:
  vector<string> args;
  bool mustExit;
  bool errorOccured;
  SemanticName semName;
  TaskType taskType;
  InstanceFormat instanceFormat;
  string instanceFile;
  map<string,string> additionalParams;

  bool assertWellFormed(bool test);
};


}


#endif
