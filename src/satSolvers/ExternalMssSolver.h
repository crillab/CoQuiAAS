/*
 * ExternalMssSolver.h
 *
 *  Created on: 21 juil. 2016
 *      Author: lonca
 */

#ifndef SRC_SOLVERS_EXTERNALMSSSOLVER_H_
#define SRC_SOLVERS_EXTERNALMSSSOLVER_H_


#define EXTERNAL_COMSS_BUFFER_SIZE (1<<12)

#include <unistd.h>
#include <sys/wait.h>

#include "MssSolver.h"
#include "ExternalSatSolver.h"


namespace CoQuiAAS {


class ExternalMssSolver: public MssSolver, public ExternalSatSolver {

public:

	ExternalMssSolver(std::string command);

	virtual void addSoftClause(std::vector<int> &clause);

	virtual bool computeMss();

	virtual bool computeMss(std::vector<int> &assumps);

	virtual bool computeMss(std::vector<int> &assumps, bool clearMssVec);

	virtual void computeAllMss();

	virtual void computeAllMss(std::vector<int> &assumps);

	virtual bool hasAMss();

	virtual std::vector<int>& getMss();

	virtual std::vector<std::vector<int> >& getAllMss();

	virtual ~ExternalMssSolver();

	virtual void addVariables(int nVars);
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

private:

	std::stringstream dimacsSoftCstrs;

	std::vector<std::vector<int> > mss;

	int nSoftCstrs;

	bool handleForkAncestor(int pipe[]);

	void extractMss(char buffer[], FILE *childOutFile);

	void clearMss();

	bool computingModel;
};


}


#endif /* SRC_SOLVERS_EXTERNALMSSSOLVER_H_ */
