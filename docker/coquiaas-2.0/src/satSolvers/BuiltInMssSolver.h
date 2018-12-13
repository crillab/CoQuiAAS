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

	virtual bool computeMss();

	virtual bool computeMss(std::vector<int> &assumps);

	virtual void computeAllMss(void (*callback)(std::vector<int>&));

	virtual void computeAllMss(void (*callback)(std::vector<int>&), std::vector<int> &assumps);

	virtual void stopMssEnum() = 0;

	virtual bool hasAMss();

	virtual std::vector<int>& getMss();

	virtual std::vector<std::vector<int> >& getAllMss();

	virtual ~BuiltInMssSolver();

	virtual void addVariables(int nVars);
	virtual bool addClause(std::vector<int> &clause);
	virtual int addSelectedClause(std::vector<int> &clause);
	virtual std::vector<int>& propagatedAtDecisionLvlZero();
	virtual bool isPropagatedAtDecisionLvlZero(int lit);
	virtual bool computeModel();
	virtual bool computeModel(std::vector<int> &assumps);
	virtual void computeAllModels(void (*callback)(std::vector<bool>& model));
	virtual void computeAllModels(void (*callback)(std::vector<bool>& model), std::vector<int> &assumps);
	virtual bool hasAModel();
	virtual std::vector<bool>& getModel();
	virtual std::vector<std::vector<bool> >& getModels();

private:

	std::vector<std::vector<int> > mss;

	void clearMss();

	int nSoftCstrs;

	bool shouldStopMssEnum = false;
};


}


#endif /* SRC_SOLVERS_BUILTINMSSSOLVER_H_ */
