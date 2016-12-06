/*
 * BuiltInMssSolver.h
 *
 *  Created on: 21 juil. 2016
 *      Author: lonca
 */

#ifndef SRC_SOLVERS_BUILTINMSSSOLVER_H_
#define SRC_SOLVERS_BUILTINMSSSOLVER_H_


#include "MssSolver.h"
#include "BuiltInSatSolver.h"


namespace CoQuiAAS {


class BuiltInMssSolver : public MssSolver, public BuiltInSatSolver {

public:

	BuiltInMssSolver();

	virtual void addSoftClause(std::vector<int> &clause);

	virtual void computeMss();

	virtual void computeMss(std::vector<int> &assumps);

	virtual void computeAllMss();

	virtual void computeAllMss(std::vector<int> &assumps);

	virtual bool hasAMss();

	virtual std::vector<int>& getMss();

	virtual std::vector<std::vector<int> >& getAllMss();

	virtual ~BuiltInMssSolver();

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

private:

	std::vector<std::vector<int> > mss;

	int nSoftCstrs;
};


}


#endif /* SRC_SOLVERS_BUILTINMSSSOLVER_H_ */
