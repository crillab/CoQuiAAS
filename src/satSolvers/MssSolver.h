/*
 * MssSolver.h
 *
 *  Created on: 21 juil. 2016
 *      Author: lonca
 */

#ifndef SRC_SOLVERS_MSSSOLVER_H_
#define SRC_SOLVERS_MSSSOLVER_H_

#include "SatSolver.h"


namespace CoQuiAAS {


class MssSolver : public SatSolver {

public:

	virtual void addSoftClause(std::vector<int> &clause) = 0;

	virtual bool computeMss() = 0;

	virtual bool computeMss(std::vector<int> &assumps) = 0;

	virtual void computeAllMss() = 0;

	virtual void computeAllMss(std::vector<int> &assumps) = 0;

	virtual bool hasAMss() = 0;

	virtual std::vector<int>& getMss() = 0;

	virtual std::vector<std::vector<int> >& getAllMss() = 0;

	virtual void resetAllMss() = 0;

	static std::vector<int> extractMssFromCoMss(Minisat::vec<int>& coMss, int nSoftCstrs) {
		std::vector<int> tmpmss;
		for(int j=1; j<=nSoftCstrs; ++j) tmpmss.push_back(j);
		for(int j = 0 ; j < coMss.size() ; j++){
			tmpmss[coMss[j]-1] = -1;
		}
		std::vector<int> mss;
		for(unsigned int j=0; j<tmpmss.size(); ++j) {
			if(tmpmss[j] < 0) continue;
			mss.push_back(tmpmss[j]);
		}
		return mss;
	}
};


}


#endif /* SRC_SOLVERS_MSSSOLVER_H_ */
