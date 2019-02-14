/**
 * \file VarMap.h
 * \brief Mapping between the names of the arguments in the input file and the associated Boolean variables in the solver
 * \author Jean-Marie Lagniez, Emmanuel Lonca, Jean-Guy Mailly
 * \version 0.1
 * \date 7/11/2014
 *
 */


#ifndef __VAR_MAP_H__
#define __VAR_MAP_H__

#include "Logger.h"
#include <vector>
#include <map>
#include <iterator>
#include <string>
#include <cassert>


namespace CoQuiAAS {


/**
 * \class VarMap
 * \brief Mapping between the names of the arguments in the input file and the Boolean variables in the solver
 */
class VarMap {

 public:
  /**
   * \fn VarMap
   * \brief Constructor
   */
  VarMap();

  /**
   * \fn addEntry
   * \brief Adds a new variables in the map
   * \param name : the name of the new variable
   */
  void addEntry(std::string name);

  void addEntry(std::vector<std::string> names);

  /**
   * \fn getName
   * \brief Gives the name of a Boolean variable
   * \param var : the Boolean variable
   * \return the name corresponding to var
   */
  std::string getName(int var);

  std::vector<std::string> getNames();

  /**
   * \fn getVar
   * \brief Gives the Boolean variable associated to a name
   * \param name : the name of the variable
   * \return the Boolean variable associated to name
   */
  int getVar(std::string name);

  /**
   * \fn contains
   * \brief Checks if the map contains a variables corresponding to a name
   * \param name : the name to be checked
   * \return true iff name corresponds to a variable
   */
  bool contains(std::string name);

  /**
   * \fn intVars
   * \brief To iterate over the integer variables corresponding to the string variables
   * \return a vector containing the integer variables
   */
  std::vector<int>& intVars();

  /**
   * \fn nVars
   * \brief Returns the number of variables the map contains
   * \return an integer indicating the number of variables the map contains
   */
  long nVars();

  /**
   * \fn isSelfAttacking(int)
   * \brief To know if the argument is self-attacking
   * \param var : the variable representing the argument
   * \return true if var is self attacking
   */
  bool isSelfAttacking(int var);

  /**
   * \fn isSelfAttacking(std::string)
   * \brief To know if the argument is self-attacking
   * \param name : the name of the argument
   * \return true if name is self attacking
   */
  bool isSelfAttacking(std::string name);


 /**
   * \fn setSelfAttacking(int, bool)
   * \brief Allows to decide if the argument is self-attacking
   * \param var : the variable representing the argument
   * \param b : the boolean true if var is self-attacking
   */
  void setSelfAttacking(int var, bool b);

  /**
   * \fn setSelfAttacking(std::string, bool)
   * \brief To know if the argument is self-attacking
   * \param name : the name of the argument
   * \param b : the boolean true if var is self-attacking
   */
  void setSelfAttacking(std::string name, bool b);

  /**
   * \fn nSelfAttacking
   * \brief Tells the number of variables which are self-attacking
   * \return the number of variables which are self-attacking
   */
  unsigned int nSelfAttacking();
  
  
 private:
  std::map<int, std::string> varToName; /**< Mapping from Boolean variables to their names */
  std::vector<int> intVariables;
  std::map<std::string, int> nameToVar; /**< Mapping from the names to the Boolean variables */
  int nvars; /**< The number of variables */
  std::map<int, bool> selfAttacking; /**< Allows to know if an argument is self-attacking */
  unsigned int nbSelfAttacking; /**< Tells the nimber of variables which are self-attacking */
};


}


#endif
