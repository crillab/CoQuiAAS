/*
 * DefaultIdealSemanticsSolver.h
 *
 *  Created on: 6 déc. 2016
 *      Author: lonca
 */

#ifndef SRC_ARGSOLVERS_DEFAULTIDEALSEMANTICSSOLVER_H_
#define SRC_ARGSOLVERS_DEFAULTIDEALSEMANTICSSOLVER_H_


#include "SemanticsProblemSolver.h"
#include "core/Solver.h"
#include "MssEncodingHelper.h"
#include "MssSolver.h"


namespace CoQuiAAS {


class DefaultIdealSemanticsSolver : public SemanticsProblemSolver {

public:
	DefaultIdealSemanticsSolver(MssSolver &solver, Attacks &attacks, VarMap &varMap, TaskType taskType);

	void init();

	void computeOneExtension();

	void computeAllExtensions();

	void isCredulouslyAccepted();

	void isSkepticallyAccepted();

	virtual ~DefaultIdealSemanticsSolver();

private:

	MssSolver &solver;
};


}


#endif /* SRC_ARGSOLVERS_DEFAULTIDEALSEMANTICSSOLVER_H_ */
