/*
 * DefaultCompleteSemanticsSolver.h
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */

#ifndef SRC_SOLVERS_DEFAULTCOMPLETESEMANTICSSOLVER_H_
#define SRC_SOLVERS_DEFAULTCOMPLETESEMANTICSSOLVER_H_


#include "SemanticsProblemSolver.h"
#include "core/Solver.h"
#include "SatEncodingHelper.h"


namespace CoQuiAAS {


class DefaultCompleteSemanticsSolver : public SemanticsProblemSolver {

public:

	DefaultCompleteSemanticsSolver(std::shared_ptr<SatSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType);

	void init();

	void computeOneExtension();

	void computeAllExtensions();

	void isCredulouslyAccepted();

	void isSkepticallyAccepted();

	virtual ~DefaultCompleteSemanticsSolver();

private:

	std::shared_ptr<SatSolver> solver;
};


}


#endif /* SRC_SOLVERS_DEFAULTCOMPLETESEMANTICSSOLVER_H_ */
