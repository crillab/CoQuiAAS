/*
 * BuiltInSatSolver.h
 *
 *  Created on: 21 juil. 2016
 *      Author: lonca
 */

#ifndef SRC_SOLVERS_BUILTINSATSOLVER_NG_GLUCOSE_H_
#define SRC_SOLVERS_BUILTINSATSOLVER_NG_GLUCOSE_H_


#define GLUCOSE_LBOOL_TRUE (Glucose::lbool((uint8_t)0))
#define GLUCOSE_LBOOL_FALSE (Glucose::lbool((uint8_t)1))
#define GLUCOSE_LBOOL_UNDEF (Glucose::lbool((uint8_t)2))


#include <iostream>

#include "SatSolver.h"
#include "cmp/ifaces/SatSolver.h"
#include "cmp/utils/wcnf.h"
#include "glucose/GlucoseSolver.h"
#include "Logger.h"


namespace CoQuiAAS {


class BuiltInSatSolverNGGlucose: public SatSolver {

public:

	BuiltInSatSolverNGGlucose();

	virtual void addVariables(int nVars);

	virtual void addVariables(int nVars, bool auxVar);

	virtual bool addClause(std::vector<int> &clause);

	virtual int addSelectedClause(std::vector<int> &clause);

	virtual std::vector<int>& propagatedAtDecisionLvlZero();

	virtual std::vector<int>& propagatedAtDecisionLvlZero(std::vector<int> assumps);

	std::vector<int>& propagatedAtDecisionLvlZero(std::vector<int> assumps, bool includeNegs);

	virtual bool isPropagatedAtDecisionLvlZero(int lit);

	virtual bool isPropagatedAtDecisionLvlZero(int lit, std::vector<int> assumps);

	virtual bool computeModel();

	virtual bool computeModel(std::vector<int> &assumps);

	virtual bool computeModel(std::vector<int> &assumps, bool clearModelVec);

	virtual void computeAllModels(std::function<void(std::vector<bool>&)> callback);

	virtual void computeAllModels(std::function<void(std::vector<bool>&)> callback, std::vector<int> &assumps);

	virtual void computeAllModels(std::function<void(std::vector<bool>&)> callback, std::vector<int> &assumps, std::vector<bool> knownModel);

	virtual bool hasAModel();

	virtual std::vector<bool>& getModel();

	virtual std::vector<std::vector<bool> >& getModels();

	virtual void resetModels();

	virtual ~BuiltInSatSolverNGGlucose();

protected:

	GlucoseSolver* solver = NULL;

	WCNF formula;

	WCNF newFormula;

	std::vector<std::vector<bool> > models;

	std::vector<int> blockingSelectors;

	std::vector<int> propagated;

	void toCmpClause(std::vector<int> &clause, CMP::vec<CMP::Lit>& cmpCl);

	void extractBuiltInSolverModel();

	int addBlockingClause();

	void clearModels();

	int realNVars = 0;

private:

	void buildSolver();

	void resetSolverState();

};

}


#endif /* SRC_SOLVERS_BUILTINSATSOLVER_NG_H_ */
