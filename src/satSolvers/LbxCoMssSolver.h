/*
 * LbxCoMssSolver.h
 *
 *  Created on: 14 déc. 2016
 *      Author: lonca
 */

#ifndef SRC_SATSOLVERS_LBXCOMSSSOLVER_H_
#define SRC_SATSOLVERS_LBXCOMSSSOLVER_H_

#include <fstream>
#include <sstream>
#include <cstring>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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

	virtual void resetAllMss();

	virtual void addVariables(int nVars);

	virtual void addVariables(int nVars, bool auxVar);

	virtual bool addClause(std::vector<int> &clause);

	virtual int addSelectedClause(std::vector<int> &clause);

	virtual std::vector<int>& propagatedAtDecisionLvlZero();

	virtual bool isPropagatedAtDecisionLvlZero(int lit);

	virtual bool computeModel();

	virtual bool computeModel(std::vector<int> &assumps);

	virtual void computeAllModels();

	virtual void computeAllModels(std::vector<int> &assumps);

	virtual bool hasAModel();

	virtual std::vector<bool>& getModel();

	virtual std::vector<std::vector<bool> >& getModels();

	virtual void resetModels();

private:

	static const int BUF_READ_SIZE = (1<<10);

	std::string lbxPath;

	std::stringstream dimacsSoftCstrs;

	std::vector<int> blockingSelectors;

	std::vector<std::vector<int> > mss;

	std::vector<std::vector<bool> >models;

	int nSoftCstrs;

	std::string writeInstanceForMSS(std::vector<int> assumps);

	std::string writeInstanceForSAT(std::vector<int> assumps, int realNumberOfVars);

	bool launchExternalSolver(std::string instanceFile, bool allModels);

	bool handleForkAncestor(int pipe[]);

	void handleForkChild(std::string instanceFile, bool allModels, int pfds[]);

	std::vector<int> extractCoMss(char *line);

	std::vector<bool> extractModel(char *line);

	std::vector<int> readIntVector(char *line);

	int nVars;

	int realNumberOfVars;

	int nCstrs;

	std::stringstream dimacsCstrs;
};

}



#endif /* SRC_SATSOLVERS_LBXCOMSSSOLVER_H_ */
