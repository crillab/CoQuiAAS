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

#include <vector>
#include <map>
#include <iterator>
#include <string>
#include <cassert>

using namespace std;

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
  void addEntry(string name);

  /**
   * \fn getName
   * \brief Gives the name of a Boolean variable
   * \param var : the Boolean variable
   * \return the name corresponding to var
   */
  string getName(int var);

  /**
   * \fn getVar
   * \brief Gives the Boolean variable associated to a name
   * \param name : the name of the variable
   * \return the Boolean variable associated to name
   */
  int getVar(string name);

  /**
   * \fn contains
   * \brief Checks if the map contains a variables corresponding to a name
   * \param name : the name to be checked
   * \return true iff name corresponds to a variable
   */
  bool contains(string name);

  /**
   * \fn intVars
   * \brief To iterate over the integer variables corresponding to the string variables
   * \return a vector containing the integer variables
   */
  vector<int>* intVars();

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
   * \fn isSelfAttacking(string)
   * \brief To know if the argument is self-attacking
   * \param name : the name of the argument
   * \return true if name is self attacking
   */
  bool isSelfAttacking(string name);


 /**
   * \fn setSelfAttacking(int, bool)
   * \brief Allows to decide if the argument is self-attacking
   * \param var : the variable representing the argument
   * \param b : the boolean true if var is self-attacking
   */
  void setSelfAttacking(int var, bool b);

  /**
   * \fn setSelfAttacking(string, bool)
   * \brief To know if the argument is self-attacking
   * \param name : the name of the argument
   * \param b : the boolean true if var is self-attacking
   */
  void setSelfAttacking(string name, bool b);

  /**
   * \fn nSelfAttacking
   * \brief Tells the number of variables which are self-attacking
   * \return the number of variables which are self-attacking
   */
  unsigned int nSelfAttacking();
  
  
 private:
  map<int, string> varToName; /**< Mapping from Boolean variables to their names */
  map<string, int> nameToVar; /**< Mapping from the names to the Boolean variables */
  int nvars; /**< The number of variables */
  map<int,bool> selfAttacking; /**< Allows to know if an argument is self-attacking */
  unsigned int nbSelfAttacking; /**< Tells the nimber of variables which are self-attacking */
};


#endif
