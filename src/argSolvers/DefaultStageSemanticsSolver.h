/*
 * DefaultStageSemanticsSolver.h
 *
 *  Created on: 5 déc. 2016
 *      Author: lonca
 */

#ifndef SRC_ARGSOLVERS_DEFAULTSTAGESEMANTICSSOLVER_H_
#define SRC_ARGSOLVERS_DEFAULTSTAGESEMANTICSSOLVER_H_


#include "SemanticsProblemSolver.h"
#include "MssEncodingHelper.h"
#include "MssSolver.h"


namespace CoQuiAAS {


class DefaultStageSemanticsSolver : public SemanticsProblemSolver {

public:
	DefaultStageSemanticsSolver(std::shared_ptr<MssSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter);

	void init();

	void computeOneExtension();

	void computeAllExtensions();

	void isCredulouslyAccepted();

	void isSkepticallyAccepted();

	virtual ~DefaultStageSemanticsSolver();

private:

	std::shared_ptr<MssSolver> solver;

	std::vector<std::vector<bool>> computeAllStgExtensions(std::function<void(std::vector<bool>&)> callback);

	bool stopEnum = false;

	MssEncodingHelper* helper;

};


}


#endif /* SRC_ARGSOLVERS_DEFAULTSTAGESEMANTICSSOLVER_H_ */
