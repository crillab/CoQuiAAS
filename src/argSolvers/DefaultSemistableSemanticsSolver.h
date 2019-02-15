/*
 * DefaultSemistableSemanticsSolver.h
 *
 *  Created on: 5 déc. 2016
 *      Author: lonca
 */

#ifndef SRC_ARGSOLVERS_DEFAULTSEMISTABLESEMANTICSSOLVER_H_
#define SRC_ARGSOLVERS_DEFAULTSEMISTABLESEMANTICSSOLVER_H_


#include "DefaultRangeBasedSemanticsSolver.h"
#include "DefaultCompleteSemanticsSolver.h"


namespace CoQuiAAS {


class DefaultSemistableSemanticsSolver : public DefaultRangeBasedSemanticsSolver {

public:
	DefaultSemistableSemanticsSolver(std::shared_ptr<MssSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter);

	virtual void init();

	void computeAllExtensions(){DefaultRangeBasedSemanticsSolver::computeAllExtensions();};

	void isCredulouslyAccepted(){DefaultRangeBasedSemanticsSolver::isCredulouslyAccepted();};

	void isSkepticallyAccepted(){DefaultRangeBasedSemanticsSolver::isSkepticallyAccepted();};

};


}


#endif /* SRC_ARGSOLVERS_DEFAULTSEMISTABLESEMANTICSSOLVER_H_ */
