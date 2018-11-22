/*
 * DefaultGroundedSemanticsSolver.h
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */

#ifndef SRC_SOLVERS_DEFAULTGROUNDEDSEMANTICSSOLVER_H_
#define SRC_SOLVERS_DEFAULTGROUNDEDSEMANTICSSOLVER_H_


#include "SemanticsProblemSolver.h"
#include "core/Solver.h"
#include "SatEncodingHelper.h"


namespace CoQuiAAS {


class DefaultGroundedSemanticsSolver : public SemanticsProblemSolver {

public:

	DefaultGroundedSemanticsSolver(std::shared_ptr<SatSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter);

	void init();

	void computeOneExtension();

	void computeAllExtensions();

	void isCredulouslyAccepted();

	void isSkepticallyAccepted();

	virtual ~DefaultGroundedSemanticsSolver();

private:

	std::shared_ptr<SatSolver> solver;
};


}


#endif /* SRC_SOLVERS_DEFAULTGROUNDEDSEMANTICSSOLVER_H_ */
