/*
 * ExternalMaxSatSolver.h
 *
 *  Created on: 5 déc. 2016
 *      Author: lonca
 */

#ifndef SRC_SATSOLVERS_EXTERNALMAXSATSOLVER_H_
#define SRC_SATSOLVERS_EXTERNALMAXSATSOLVER_H_

#include "MaxSatSolver.h"
#include "ExternalSatSolver.h"


#define EXTERNAL_MAXSAT_BUFFER_SIZE (1<<12)


namespace CoQuiAAS {

class ExternalMaxSatSolver : public MaxSatSolver, public ExternalSatSolver {

public :

	ExternalMaxSatSolver(std::string command);

	virtual ~ExternalMaxSatSolver();

	virtual void addSoftClause(std::vector<int> &clause);

	virtual void computeMaxSat();

	virtual void computeMaxSat(std::vector<int> &assumps);

	virtual void computeAllMaxSat();

	virtual void computeAllMaxSat(std::vector<int> &assumps);

	virtual std::vector<bool>& getMaxSat();

	virtual std::vector<std::vector<bool> >& getAllMaxSat();

	virtual int getOptValue();

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

	void extractMaxSat(char buffer[], FILE *childOutFile);

	int optValue;

	int lastObjValue;

	std::stringstream dimacsSoftCstrs;

	int nSoftCstrs;

	void handleForkAncestor(int pipe[]);

	void extractMss(char buffer[], FILE *childOutFile);

	bool computingModel;
};

}


#endif /* SRC_SATSOLVERS_EXTERNALMAXSATSOLVER_H_ */
