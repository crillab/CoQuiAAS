/*
 * DefaultPreferredSemanticsSolver.h
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */


#ifndef SRC_SOLVERS_DEFAULTPREFERREDSEMANTICSSOLVER_H_
#define SRC_SOLVERS_DEFAULTPREFERREDSEMANTICSSOLVER_H_


#include "SemanticsProblemSolver.h"
#include "core/Solver.h"
#include "MssEncodingHelper.h"
#include "MssSolver.h"


class DefaultPreferredSemanticsSolver : public SemanticsProblemSolver {

public:
	DefaultPreferredSemanticsSolver(MssSolver &solver, Attacks &attacks, VarMap &varMap, TaskType taskType);

	void init();

	void computeOneExtension();

	void computeAllExtensions();

	void isCredulouslyAccepted();

	void isSkepticallyAccepted();

	virtual ~DefaultPreferredSemanticsSolver();

private:

	MssSolver &solver;
};


#endif /* SRC_SOLVERS_DEFAULTPREFERREDSEMANTICSSOLVER_H_ */
