/**
 * \file Attacks.h
 * \brief The attack relation of the argumentation framework
 * \author Jean-Marie Lagniez, Emmanuel Lonca, Jean-Guy Mailly
 * \version 0.1
 * \date 7/11/2014
 */

#ifndef __ATTACKS_H__
#define __ATTACKS_H__

#include <map>
#include <vector>
#include <string>
#include <iostream>

#include "VarMap.h"

using namespace std;

/**
 * \class Attacks
 * \brief The attack relation of the argumentation framework
 */
class Attacks {
  
 public:
  /**
   * \fn Attacks
   * \brief Constructor
   * \param vm : the variables concerned by the attack relation
   */
  Attacks(VarMap *vm);

  /**
   * \fn addAttack
   * \brief Adds an attack in the graph
   * \param from : the name of the attacking argument
   * \param to : the name of the attacked argument
   */
  void addAttack(string from, string to);

  /**
   * \fn getAttacksTo
   * \brief Gets the list of variables corresponding to the attackers of the parameter
   * \param var : the variable
   * \return the attackers of var
   */
  vector<int> *getAttacksTo(int var);

  /**
   * \fn nAttacks
   * \brief return the number of attacks the problem contains
   * \return the number of attacks
   */
  unsigned int nAttacks();

  /**
   * \fn nAttacks
   * \brief return the maximal number of attacks an argument suffers
   * \return the maximal number of attacks an argument suffers
   */
  unsigned int maxAttacks();

 protected :
  /** The variables on which is built the attack relation */
  VarMap& varMap;

  /** The mapping from the arguments to their attackers */
  map<int,vector<int> > attacks;

 private:
  unsigned int nbAttacks;
  unsigned int nMaxAttacks;
  
};

#endif
