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

	virtual void addVariables(int nVars, bool auxVar);

	virtual bool addClause(std::vector<int> &clause);

	virtual int addSelectedClause(std::vector<int> &clause);

	virtual std::vector<int>& propagatedAtDecisionLvlZero();

	virtual std::vector<int>& propagatedAtDecisionLvlZero(std::vector<int> assumps);

	virtual bool isPropagatedAtDecisionLvlZero(int lit);

	virtual bool isPropagatedAtDecisionLvlZero(int lit, std::vector<int> assumps);

	virtual bool computeModel();

	virtual bool computeModel(std::vector<int> &assumps);

	virtual bool computeModel(std::vector<int> &assumps, bool clearModelVec);

	virtual void computeAllModels(std::function<void(std::vector<bool>&)> callback);

	virtual void computeAllModels(std::function<void(std::vector<bool>&)> callback, std::vector<int> &assumps);

	virtual bool hasAModel();

	virtual std::vector<bool>& getModel();

	virtual std::vector<std::vector<bool> >& getModels();

	virtual void resetModels();

	virtual ~ExternalSatSolver();

protected:

	int nVars;

	int nCstrs;

	std::stringstream dimacsCstrs;

	virtual bool launchExternalSolver(std::string instanceFile);

	virtual bool handleForkAncestor(int pipe[]);

	virtual void handleForkChild(std::string instanceFile, int pipe[]);

	int addBlockingClause();

	std::vector<std::vector<bool> > models;

	std::vector<int> blockingSelectors;

	void extractModel(char buffer[], FILE *childOutFile);

	void clearModels();

private :

	std::string command;

};


}


#endif /* SRC_SOLVERS_EXTERNALSATSOLVER_H_ */
