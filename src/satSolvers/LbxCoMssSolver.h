/*
 * LbxCoMssSolver.h
 *
 *  Created on: 14 déc. 2016
 *      Author: lonca
 */

#ifndef SRC_SATSOLVERS_LBXCOMSSSOLVER_H_
#define SRC_SATSOLVERS_LBXCOMSSSOLVER_H_

#include "MssSolver.h"

namespace CoQuiAAS {

class LbxCoMssSolver : public MssSolver {

public:

	LbxCoMssSolver(std::string lbxPath);

	virtual ~LbxCoMssSolver();

	virtual void addSoftClause(std::vector<int> &clause);

	void clearMss();

	virtual bool computeMss();

	virtual bool computeMss(std::vector<int> &assumps);

	virtual void computeAllMss();

	virtual void computeAllMss(std::vector<int> &assumps);

	virtual bool hasAMss();

	virtual std::vector<int>& getMss();

	virtual std::vector<std::vector<int> >& getAllMss();

private:

	std::string lbxPath;

	std::stringstream dimacsSoftCstrs;

	std::vector<int> blockingSelectors;

	std::vector<std::vector<int> > mss;

	int nSoftCstrs;

	std::string writeInstance(std::vector<int> assumps);

	bool launchExternalSolver(std::string instanceFile, bool allModels);

	void handleForkChild(std::string instanceFile, bool allModels, int pfds[])

};

}



#endif /* SRC_SATSOLVERS_LBXCOMSSSOLVER_H_ */
