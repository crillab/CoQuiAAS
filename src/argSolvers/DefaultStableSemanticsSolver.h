/*
 * StableSemanticsSolver.h
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */

#ifndef SRC_SOLVERS_STABLESEMANTICSSOLVER_H_
#define SRC_SOLVERS_STABLESEMANTICSSOLVER_H_


#include <memory>

#include "SemanticsProblemSolver.h"
#include "SatEncodingHelper.h"
#include "StableEncodingSatProblemReducer.h"


namespace CoQuiAAS {


class DefaultStableSemanticsSolver : public SemanticsProblemSolver {

public:

	DefaultStableSemanticsSolver(std::shared_ptr<SatSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter);

	void init();

	void computeOneExtension();

	void computeAllExtensions();

	void isCredulouslyAccepted();

	void isSkepticallyAccepted();

	virtual ~DefaultStableSemanticsSolver();

private:

	std::shared_ptr<SatSolver> solver;

	SatEncodingHelper* helper;

	std::unique_ptr<StableEncodingSatProblemReducer> problemReducer;
};


}


#endif /* SRC_SOLVERS_STABLESEMANTICSSOLVER_H_ */
