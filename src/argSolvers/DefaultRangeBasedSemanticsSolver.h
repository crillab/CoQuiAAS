#ifndef SRC_ARGSOLVERS_DEFAULTRANGEBASEDSEMANTICSSOLVER_H_
#define SRC_ARGSOLVERS_DEFAULTRANGEBASEDSEMANTICSSOLVER_H_


#include "SemanticsProblemSolver.h"
#include "MssEncodingHelper.h"
#include "MssSolver.h"
#include "SatProblemReducer.h"


namespace CoQuiAAS {


class DefaultRangeBasedSemanticsSolver : public SemanticsProblemSolver {

public:
	DefaultRangeBasedSemanticsSolver(std::shared_ptr<MssSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter);

	virtual void init() = 0;

	void computeOneExtension();

	void computeAllExtensions();

	void isCredulouslyAccepted();

	void isSkepticallyAccepted();

	virtual ~DefaultRangeBasedSemanticsSolver();

protected:

	std::shared_ptr<MssSolver> solver;

	MssEncodingHelper* helper;

	bool stopEnum = false;

	std::unique_ptr<SatProblemReducer> problemReducer;

private:

	std::vector<std::vector<bool>> computeAllExtensions(std::function<void(std::vector<bool>&)> callback);

};


}


#endif