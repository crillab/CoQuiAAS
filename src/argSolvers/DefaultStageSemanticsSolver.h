/*
 * DefaultStageSemanticsSolver.h
 *
 *  Created on: 5 déc. 2016
 *      Author: lonca
 */

#ifndef SRC_ARGSOLVERS_DEFAULTSTAGESEMANTICSSOLVER_H_
#define SRC_ARGSOLVERS_DEFAULTSTAGESEMANTICSSOLVER_H_


#include "DefaultRangeBasedSemanticsSolver.h"
#include "MssEncodingHelper.h"
#include "MssSolver.h"


namespace CoQuiAAS {


class DefaultStageSemanticsSolver : public DefaultRangeBasedSemanticsSolver {

public:
	DefaultStageSemanticsSolver(std::shared_ptr<MssSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter);

	virtual void init();

	void computeAllExtensions(){DefaultRangeBasedSemanticsSolver::computeAllExtensions();};

	void isCredulouslyAccepted(){DefaultRangeBasedSemanticsSolver::isCredulouslyAccepted();};

	void isSkepticallyAccepted(){DefaultRangeBasedSemanticsSolver::isSkepticallyAccepted();};

};


}


#endif /* SRC_ARGSOLVERS_DEFAULTSTAGESEMANTICSSOLVER_H_ */
