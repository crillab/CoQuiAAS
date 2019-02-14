#ifndef __SAT_PROBLEM_REDUCER_H__
#define __SAT_PROBLEM_REDUCER_H__

#include "BuiltInSatSolverNG.h"
#include "SatEncodingHelper.h"
#include "VarMap.h"
#include "Attacks.h"

#include <memory>


namespace CoQuiAAS {

class CompleteEncodingSatProblemReducer {

public:

    CompleteEncodingSatProblemReducer(VarMap& initVarMap, Attacks& initAttacks);

    inline std::vector<std::vector<int> >& getEqClasses() {
        return this->eqClasses;
    }

    inline std::shared_ptr<VarMap> getReducedMap() {
        return this->reducedVarMap;
    }

    inline std::string translateAcceptanceQueryArgument(std::string arg) {
        return this->initVarMap.getName(this->equivalentTo[this->initVarMap.getVar(arg)]);
    }

    /* inline std::shared_ptr<Attacks> getReducedAttacks() {
        return this->reducedAttacks;
    } */

    // std::vector<bool> translateModel(std::vector<bool> initModel);

private:

    void search();

    void propagateAtDecisionLvlZero();

    void lookForEquivalencesOf(int var);

    void computePropagationsOf(int var);

    void translateVarMap();

    // void translateAttacks();

    std::shared_ptr<BuiltInSatSolverNG> slv;

    VarMap& initVarMap;

    std::shared_ptr<VarMap> reducedVarMap;
    
    Attacks& initAttacks;

    // std::shared_ptr<Attacks> reducedAttacks;

    std::vector<bool> fixed;

    std::vector<bool> fixedVal;

    std::vector<int> equivalentTo;

    std::vector<std::vector<int> > eqClasses;

    std::vector<std::vector<int>* > propagated;

};

}

#endif