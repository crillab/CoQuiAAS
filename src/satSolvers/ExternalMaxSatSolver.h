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

	virtual bool computeMaxSat();

	virtual bool computeMaxSat(std::vector<int> &assumps);

	virtual bool computeMaxSat(std::vector<int> &assumps, bool clearModelVec);

	virtual void computeAllMaxSat();

	virtual void computeAllMaxSat(std::vector<int> &assumps);

	virtual std::vector<bool>& getMaxSat();

	virtual std::vector<std::vector<bool> >& getAllMaxSat();

	virtual int getOptValue();

	virtual void addVariables(int nVars);
	virtual void addVariables(int nVars, bool auxVars);
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
	virtual void computeAllModels(std::function<void(std::vector<bool>&)> callback, std::vector<int> &assumps, std::vector<bool> knownModel);
	virtual bool hasAModel();
	virtual std::vector<bool>& getModel();
	virtual std::vector<std::vector<bool> >& getModels();
	virtual void resetModels();


private:

	void extractMaxSat(char buffer[], FILE *childOutFile);

	int optValue;

	int lastObjValue;

	std::stringstream dimacsSoftCstrs;

	int nSoftCstrs;

	bool handleForkAncestor(int pipe[]);

	bool computingModel;
};

}


#endif /* SRC_SATSOLVERS_EXTERNALMAXSATSOLVER_H_ */
