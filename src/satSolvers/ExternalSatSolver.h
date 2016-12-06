/*
 * ExternalSatSolver.h
 *
 *  Created on: 21 juil. 2016
 *      Author: lonca
 */

#ifndef SRC_SOLVERS_EXTERNALSATSOLVER_H_
#define SRC_SOLVERS_EXTERNALSATSOLVER_H_


#define EXTERNAL_SAT_BUFFER_SIZE (1<<12)


#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <iterator>

#include <unistd.h>
#include <sys/wait.h>

#include "SatSolver.h"


namespace CoQuiAAS {


class ExternalSatSolver: public SatSolver {

public:

	ExternalSatSolver(std::string command);

	virtual void addVariables(int nVars);

	virtual bool addClause(std::vector<int> &clause);

	virtual int addSelectedClause(std::vector<int> &clause);

	virtual std::vector<int>& propagatedAtDecisionLvlZero();

	virtual bool isPropagatedAtDecisionLvlZero(int lit);

	virtual void computeModel();

	virtual void computeModel(std::vector<int> &assumps);

	virtual void computeAllModels();

	virtual void computeAllModels(std::vector<int> &assumps);

	virtual bool hasAModel();

	virtual std::vector<bool>& getModel();

	virtual std::vector<std::vector<bool> >& getModels();

	virtual ~ExternalSatSolver();

protected:

	int nVars;

	int nCstrs;

	std::stringstream dimacsCstrs;

	virtual void launchExternalSolver(std::string instanceFile);

	virtual void handleForkAncestor(int pipe[]);

	virtual void handleForkChild(std::string instanceFile, int pipe[]);

	int addBlockingClause();

	std::vector<std::vector<bool> > models;

	void extractModel(char buffer[], FILE *childOutFile);

private :

	std::string command;

};


}


#endif /* SRC_SOLVERS_EXTERNALSATSOLVER_H_ */
