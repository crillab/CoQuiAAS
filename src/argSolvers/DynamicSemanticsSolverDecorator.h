#ifndef __ARG_SOLVERS__DYNAMIC_SEMANTICS_SOLVER_DECORATOR_H__
#define __ARG_SOLVERS__DYNAMIC_SEMANTICS_SOLVER_DECORATOR_H__

#include "SemanticsProblemSolver.h"
#include "core/Solver.h"
#include "SatEncodingHelper.h"
#include "SolverOutputFormatter.h"

namespace CoQuiAAS {

template<typename Impl>
class DynamicSemanticsSolverDecorator: public SemanticsProblemSolver {

public:

    DynamicSemanticsSolverDecorator(Impl &decorated);

	void init();

	void computeOneExtension();

	void computeAllExtensions();

	void isCredulouslyAccepted();

	void isSkepticallyAccepted();

	virtual void setAcceptanceQueryArgument(std::string arg);

	virtual ~DynamicSemanticsSolverDecorator();

    void setDynStep(int step);

private:

	Impl& decorated;

    void iterate(std::function<void()> toCall);

};

}

using namespace CoQuiAAS;

template<typename Impl>
DynamicSemanticsSolverDecorator<Impl>::DynamicSemanticsSolverDecorator(Impl &decorated):
    SemanticsProblemSolver(decorated.getAttacks(), decorated.getVarMap(), decorated.getTaskType(), decorated.getFormatter()), decorated(decorated) {}

template<typename Impl>
void DynamicSemanticsSolverDecorator<Impl>::setAcceptanceQueryArgument(std::string arg) {
	this->decorated.setAcceptanceQueryArgument(arg);
}

template<typename Impl>
void DynamicSemanticsSolverDecorator<Impl>::init() {
    this->decorated.init();
}

template<typename Impl>
void DynamicSemanticsSolverDecorator<Impl>::computeOneExtension() {
    iterate(std::bind(&SemanticsProblemSolver::computeOneExtension, this->decorated));
}

template<typename Impl>
void DynamicSemanticsSolverDecorator<Impl>::computeAllExtensions() {
    iterate(std::bind(&SemanticsProblemSolver::computeAllExtensions, this->decorated));
}

template<typename Impl>
void DynamicSemanticsSolverDecorator<Impl>::isCredulouslyAccepted() {
    iterate(std::bind(&SemanticsProblemSolver::isCredulouslyAccepted, this->decorated));
}

template<typename Impl>
void DynamicSemanticsSolverDecorator<Impl>::isSkepticallyAccepted() {
    iterate(std::bind(&SemanticsProblemSolver::isSkepticallyAccepted, this->decorated));
}

template<typename Impl>
void DynamicSemanticsSolverDecorator<Impl>::setDynStep(int step) {
    this->decorated.setDynStep(step);
}

template<typename Impl>
void DynamicSemanticsSolverDecorator<Impl>::iterate(std::function<void()> toCall) {
    if(attacks.getDynAttacks().size() == 0) {
        toCall();
        return;
    }
    this->formatter.writeDynListBegin(this->taskType);
    toCall();
    for(unsigned int i=0; i<attacks.getDynAttacks().size(); ++i) {
        this->decorated.setDynStep(i);
        toCall();
        if(i != 0) this->formatter.writeDynListElmtSep(this->taskType);
    }
    this->formatter.writeDynListEnd(this->taskType);
}

template<typename Impl>
DynamicSemanticsSolverDecorator<Impl>::~DynamicSemanticsSolverDecorator() {}

#endif