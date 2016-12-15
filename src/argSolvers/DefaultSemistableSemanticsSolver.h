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
	DefaultSemistableSemanticsSolver(MssSolver &solver, Attacks &attacks, VarMap &varMap, TaskType taskType);

	void init();

	void computeOneExtension();

	void computeAllExtensions();

	void isCredulouslyAccepted();

	void isSkepticallyAccepted();

	virtual ~DefaultSemistableSemanticsSolver();

private:

	MssSolver &solver;

	void computeAllSstExtensions();
};


}


#endif /* SRC_ARGSOLVERS_DEFAULTSEMISTABLESEMANTICSSOLVER_H_ */
