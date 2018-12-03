/*
 * BuiltInSatSolver.h
 *
 *  Created on: 21 juil. 2016
 *      Author: lonca
 */

#ifndef SRC_SOLVERS_BUILTINSATSOLVER_H_
#define SRC_SOLVERS_BUILTINSATSOLVER_H_


#define MINISAT_LBOOL_TRUE (Minisat::lbool((uint8_t)0))
#define MINISAT_LBOOL_FALSE (Minisat::lbool((uint8_t)1))
#define MINISAT_LBOOL_UNDEF (Minisat::lbool((uint8_t)2))


#include <iostream>

#include "SatSolver.h"
#include "core/Solver.h"
#include "core/SolverTypes.h"


namespace CoQuiAAS {


class BuiltInSatSolver: public SatSolver {

public:

	BuiltInSatSolver();

	virtual void addVariables(int nVars);

	virtual void addVariables(int nVars, bool auxVar);

	virtual bool addClause(std::vector<int> &clause);

	virtual int addSelectedClause(std::vector<int> &clause);

	virtual std::vector<int>& propagatedAtDecisionLvlZero();

	virtual bool isPropagatedAtDecisionLvlZero(int lit);

	virtual bool computeModel();

	virtual bool computeModel(std::vector<int> &assumps);

	virtual bool computeModel(std::vector<int> &assumps, bool clearModelVec);

	virtual void computeAllModels(std::function<void(std::vector<bool>&)> callback);

	virtual void computeAllModels(std::function<void(std::vector<bool>&)> callback, std::vector<int> &assumps);

	virtual bool hasAModel();

	virtual std::vector<bool>& getModel();

	virtual std::vector<std::vector<bool> >& getModels();

	virtual void resetModels();

	virtual ~BuiltInSatSolver();

protected:

	Minisat::Solver solver;

	int nVars;

	int nCstrs;

	std::vector<std::vector<bool> > models;

	std::vector<int> blockingSelectors;

	std::vector<int> propagated;

	void extractBuiltInSolverModel();

	int addBlockingClause();

	void clearModels();
};


}


#endif /* SRC_SOLVERS_BUILTINSATSOLVER_H_ */
