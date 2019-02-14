/*
 * DefaultCompleteSemanticsSolver.h
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */

#ifndef SRC_SOLVERS_DEFAULTCOMPLETESEMANTICSSOLVER_H_
#define SRC_SOLVERS_DEFAULTCOMPLETESEMANTICSSOLVER_H_


#include "SemanticsProblemSolver.h"
#include "SatEncodingHelper.h"
#include "SolverOutputFormatter.h"
#include "BuiltInSatSolverNG.h"
#include "CompleteEncodingSatProblemReducer.h"


namespace CoQuiAAS {


class DefaultCompleteSemanticsSolver : public SemanticsProblemSolver {

public:

	DefaultCompleteSemanticsSolver(std::shared_ptr<SatSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter);

	void init();

	void computeOneExtension();

	void computeAllExtensions();

	void isCredulouslyAccepted();

	void isSkepticallyAccepted();

	virtual ~DefaultCompleteSemanticsSolver();

private:

	std::shared_ptr<SatSolver> solver;

	SatEncodingHelper* helper;

	std::unique_ptr<CompleteEncodingSatProblemReducer> problemReducer;
};


}


#endif /* SRC_SOLVERS_DEFAULTCOMPLETESEMANTICSSOLVER_H_ */
