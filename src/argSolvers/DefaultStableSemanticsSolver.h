/*
 * StableSemanticsSolver.h
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */

#ifndef SRC_SOLVERS_STABLESEMANTICSSOLVER_H_
#define SRC_SOLVERS_STABLESEMANTICSSOLVER_H_


#include "SemanticsProblemSolver.h"
#include "core/Solver.h"
#include "SatEncodingHelper.h"


class DefaultStableSemanticsSolver : public SemanticsProblemSolver {

public:

	DefaultStableSemanticsSolver(SatSolver &solver, Attacks &attacks, VarMap &varMap, TaskType taskType);

	void init();

	void computeOneExtension();

	void computeAllExtensions();

	void isCredulouslyAccepted();

	void isSkepticallyAccepted();

	virtual ~DefaultStableSemanticsSolver();

private:

	SatSolver &solver;
};

#endif /* SRC_SOLVERS_STABLESEMANTICSSOLVER_H_ */
