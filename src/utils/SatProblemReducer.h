#ifndef __SAT_PROBLEM_REDUCER_H__
#define __SAT_PROBLEM_REDUCER_H__

#include "BuiltInSatSolverNG.h"
#include "SatEncodingHelper.h"
#include "VarMap.h"
#include "Attacks.h"

#include <memory>


namespace CoQuiAAS {

class SatProblemReducer {

public:

    SatProblemReducer(VarMap& initVarMap, Attacks& initAttacks);

    void search();

    inline std::shared_ptr<VarMap> getReducedMap() {
        return this->reducedVarMap;
    }

    inline std::string translateAcceptanceQueryArgument(std::string arg) {
        return this->initVarMap.getName(this->equivalentTo[this->initVarMap.getVar(arg)]);
    }

    inline bool isFixedFalse(std::string arg) {
        int var = this->initVarMap.getVar(arg);
        return this->fixed[var] && !this->fixedVal[var];
    }

    inline bool isFixedTrue(std::string arg) {
        int var = this->initVarMap.getVar(arg);
        return this->fixed[var] && this->fixedVal[var];
    }

protected:

    virtual void init() = 0;

    void propagateAtDecisionLvlZero();

    void lookForEquivalencesOf(int var);

    void computePropagationsOf(int var);

    void translateVarMap();

    std::shared_ptr<BuiltInSatSolverNG> slv;

    VarMap& initVarMap;

    std::shared_ptr<VarMap> reducedVarMap;
    
    Attacks& initAttacks;

    std::vector<bool> fixed;

    std::vector<bool> fixedVal;

    std::vector<int> equivalentTo;

    std::vector<std::vector<int> > eqClasses;

    std::vector<std::vector<int>* > propagated;

};

}

#endif