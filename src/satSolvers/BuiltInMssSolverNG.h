/*
 * BuiltInMssSolver.h
 *
 *  Created on: 21 juil. 2016
 *      Author: lonca
 */

#ifndef SRC_SOLVERS_BUILTINMSSSOLVER_H_
#define SRC_SOLVERS_BUILTINMSSSOLVER_H_


#include "MssSolver.h"
#include "BuiltInSatSolverNG.h"

#include "cmp/core/constructCoMSSExtract.h"
#include "cmp/enum/coMSSEnum.h"


namespace CoQuiAAS {


class BuiltInMssSolverNG : public MssSolver, public BuiltInSatSolverNG {

public:

	BuiltInMssSolverNG();

	virtual void addSoftClause(std::vector<int> &clause);

	virtual bool computeMss();

	virtual bool computeMss(std::vector<int> &assumps);

	virtual void computeAllMss(std::function<void(std::vector<int>&)> callback);

	virtual void computeAllMss(std::function<void(std::vector<int>&)> callback, std::vector<int> &assumps);

	virtual void stopMssEnum();

	virtual bool hasAMss();

	virtual std::vector<int>& getMss();

	virtual std::vector<std::vector<int> >& getAllMss();

	virtual ~BuiltInMssSolverNG();

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
	virtual void computeAllModels(std::function<void(std::vector<bool>&)> callback);
	virtual void computeAllModels(std::function<void(std::vector<bool>&)> callback, std::vector<int> &assumps);
	virtual bool hasAModel();
	virtual std::vector<bool>& getModel();
	virtual std::vector<std::vector<bool> >& getModels();
	virtual void resetAllMss();
	virtual void resetModels();

private:

	CoMSSExtract *mcsFinder = NULL;

	CoMSSEnum *mcsEnumerator = NULL;

	WCNF formula;

	std::vector<std::vector<int> > mss;

	void clearMss();

	int nSoftCstrs;

	bool shouldStopMssEnum = false;

	void toCmpClause(std::vector<int> &clause, CMP::vec<CMP::Lit>& cmpCl);

	static std::vector<int> extractMssFromCoMss(CMP::vec<CMP::Lit>& coMss, int nSoftCstrs) {
		std::vector<int> tmpmss;
		for(int j=1; j<=nSoftCstrs; ++j) tmpmss.push_back(j);
		for(int j = 0 ; j < coMss.size() ; j++){
			tmpmss[CMP::var(coMss[j])] = -1;
		}
		std::vector<int> mss;
		for(unsigned int j=0; j<tmpmss.size(); ++j) {
			if(tmpmss[j] < 0) continue;
			mss.push_back(tmpmss[j]);
		}
		return mss;
	}

	void computeSomeMsses(std::function<void(std::vector<int>&)> callback, std::vector<int> &assumps, int maxCount);
};


}


#endif /* SRC_SOLVERS_BUILTINMSSSOLVER_H_ */
