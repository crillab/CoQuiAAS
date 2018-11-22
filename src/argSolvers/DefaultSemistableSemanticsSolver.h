/*
 * DefaultSemistableSemanticsSolver.h
 *
 *  Created on: 5 déc. 2016
 *      Author: lonca
 */

#ifndef SRC_ARGSOLVERS_DEFAULTSEMISTABLESEMANTICSSOLVER_H_
#define SRC_ARGSOLVERS_DEFAULTSEMISTABLESEMANTICSSOLVER_H_


#include "SemanticsProblemSolver.h"
#include "core/Solver.h"
#include "MssEncodingHelper.h"
#include "MssSolver.h"


namespace CoQuiAAS {


class DefaultSemistableSemanticsSolver : public SemanticsProblemSolver {

public:
	DefaultSemistableSemanticsSolver(std::shared_ptr<MssSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter);

	void init();

	void computeOneExtension();

	void computeAllExtensions();

	void isCredulouslyAccepted();

	void isSkepticallyAccepted();

	virtual ~DefaultSemistableSemanticsSolver();

private:

	std::shared_ptr<MssSolver> solver;

	std::vector<std::vector<bool>> computeAllSstExtensions();
};


}


#endif /* SRC_ARGSOLVERS_DEFAULTSEMISTABLESEMANTICSSOLVER_H_ */
