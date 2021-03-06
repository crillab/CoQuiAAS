/*
 * DefaultIdealSemanticsSolver.h
 *
 *  Created on: 6 déc. 2016
 *      Author: lonca
 */

#ifndef SRC_ARGSOLVERS_DEFAULTIDEALSEMANTICSSOLVER_H_
#define SRC_ARGSOLVERS_DEFAULTIDEALSEMANTICSSOLVER_H_


#include "SemanticsProblemSolver.h"
#include "MssEncodingHelper.h"
#include "MssSolver.h"
#include "CompleteEncodingSatProblemReducer.h"


namespace CoQuiAAS {


class DefaultIdealSemanticsSolver : public SemanticsProblemSolver {

public:
	DefaultIdealSemanticsSolver(std::shared_ptr<MssSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter);

	void init();

	void computeOneExtension();

	void computeAllExtensions();

	void isCredulouslyAccepted();

	void isSkepticallyAccepted();

	virtual ~DefaultIdealSemanticsSolver();

private:

	std::vector<int> justComputeOneExtension(std::function<void(std::vector<int>&)> prExtCallback);

	std::shared_ptr<MssSolver> solver;

	MssEncodingHelper* helper;

	std::unique_ptr<CompleteEncodingSatProblemReducer> problemReducer;

	bool stopSearch = false;
};


}


#endif /* SRC_ARGSOLVERS_DEFAULTIDEALSEMANTICSSOLVER_H_ */
