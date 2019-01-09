#ifndef __ARG_SOLVERS__DYNAMIC_SEMANTICS_SOLVER_DECORATOR_H__
#define __ARG_SOLVERS__DYNAMIC_SEMANTICS_SOLVER_DECORATOR_H__

#include "SemanticsProblemSolver.h"
#include "SatEncodingHelper.h"
#include "SolverOutputFormatter.h"
#include "DefaultRangeBasedSemanticsSolver.h"
#include "DefaultSemistableSemanticsSolver.h"

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

    virtual void setDynStep(int step);

private:

	Impl& decorated;

    void iterate(std::function<void(Impl&)> toCall);

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
    iterate(&Impl::computeOneExtension);
}

template<typename Impl>
void DynamicSemanticsSolverDecorator<Impl>::computeAllExtensions() {
    iterate(&Impl::computeAllExtensions);
}

template<typename Impl>
void DynamicSemanticsSolverDecorator<Impl>::isCredulouslyAccepted() {
    iterate(&Impl::isCredulouslyAccepted);
}

template<typename Impl>
void DynamicSemanticsSolverDecorator<Impl>::isSkepticallyAccepted() {
    iterate(&Impl::isSkepticallyAccepted);
}

template<typename Impl>
void DynamicSemanticsSolverDecorator<Impl>::setDynStep(int step) {
    this->decorated.setDynStep(step);
}

template<typename Impl>
void DynamicSemanticsSolverDecorator<Impl>::iterate(std::function<void(Impl&)> toCall) {
    this->formatter.writeDynListBegin(this->taskType);
    toCall(this->decorated);
    for(unsigned int i=0; i<attacks.getDynAttacks().size(); ++i) {
        this->formatter.writeDynListElmtSep(this->taskType);
        this->decorated.setDynStep(i);
        toCall(this->decorated);
    }
    this->formatter.writeDynListEnd(this->taskType);
}

template<typename Impl>
DynamicSemanticsSolverDecorator<Impl>::~DynamicSemanticsSolverDecorator() {}

#endif