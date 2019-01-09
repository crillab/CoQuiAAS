/*
 * BuiltInSatSolver.h
 *
 *  Created on: 21 juil. 2016
 *      Author: lonca
 */

#ifndef SRC_SOLVERS_BUILTINSATSOLVER_NG_H_
#define SRC_SOLVERS_BUILTINSATSOLVER_NG_H_


#define MINISAT_LBOOL_TRUE (Minisat::lbool((uint8_t)0))
#define MINISAT_LBOOL_FALSE (Minisat::lbool((uint8_t)1))
#define MINISAT_LBOOL_UNDEF (Minisat::lbool((uint8_t)2))


#include <iostream>

#include "SatSolver.h"
#include "cmp/ifaces/SatSolver.h"
#include "cmp/utils/wcnf.h"
#include "minisat/MiniSatSolver.h"


namespace CoQuiAAS {


class BuiltInSatSolverNG: public SatSolver {

public:

	BuiltInSatSolverNG();

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

	virtual ~BuiltInSatSolverNG();

protected:

	MiniSatSolver* solver;

	WCNF formula;

	WCNF newFormula;

	std::vector<std::vector<bool> > models;

	std::vector<int> blockingSelectors;

	std::vector<int> propagated;

	void toCmpClause(std::vector<int> &clause, CMP::vec<CMP::Lit>& cmpCl);

	void extractBuiltInSolverModel();

	int addBlockingClause();

	void clearModels();

private:

	void buildSolver();

	int realNVars = 0;

};

}


#endif /* SRC_SOLVERS_BUILTINSATSOLVER_NG_H_ */
