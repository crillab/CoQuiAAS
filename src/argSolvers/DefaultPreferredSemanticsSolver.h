/*
 * DefaultPreferredSemanticsSolver.h
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */


#ifndef SRC_SOLVERS_DEFAULTPREFERREDSEMANTICSSOLVER_H_
#define SRC_SOLVERS_DEFAULTPREFERREDSEMANTICSSOLVER_H_


#include "SemanticsProblemSolver.h"
#include "MssEncodingHelper.h"
#include "MssSolver.h"


namespace CoQuiAAS {


class DefaultPreferredSemanticsSolver : public SemanticsProblemSolver {

public:
	DefaultPreferredSemanticsSolver(std::shared_ptr<MssSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter);

	void init();

	void computeOneExtension();

	void computeAllExtensions();

	void isCredulouslyAccepted();

	void isSkepticallyAccepted();

	virtual ~DefaultPreferredSemanticsSolver();

private:

	std::shared_ptr<MssSolver> solver;

	MssEncodingHelper* helper;
};


}


#endif /* SRC_SOLVERS_DEFAULTPREFERREDSEMANTICSSOLVER_H_ */
