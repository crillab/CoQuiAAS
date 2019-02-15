#include "CompleteEncodingSatProblemReducer.h"

using namespace CoQuiAAS;


CompleteEncodingSatProblemReducer::CompleteEncodingSatProblemReducer(VarMap& initVarMap, Attacks& initAttacks): initVarMap(initVarMap), initAttacks(initAttacks) {
    this->slv = std::make_shared<BuiltInSatSolverNG>();
	SatEncodingHelper helper(this->slv, this->initAttacks, this->initVarMap);
	int disjId0 = helper.reserveDisjunctionVars();
	helper.createAttackersDisjunctionVars(disjId0);
	helper.createCompleteEncodingConstraints(disjId0);
	search();
    translateVarMap();
}

void CompleteEncodingSatProblemReducer::search() {
    int nVars = this->initVarMap.nVars();
    this->fixed = std::vector<bool>(1+nVars, false);
    this->fixedVal = std::vector<bool>(1+nVars);
    for(int i=0; i<=nVars; ++i) this->equivalentTo.push_back(i);
    this->eqClasses.clear();
    propagateAtDecisionLvlZero();
    for(int i=0; i<=nVars; ++i) this->propagated.push_back(NULL);
    for(int i=1; i<=nVars; ++i) {
        if(!this->fixed[i]) lookForEquivalencesOf(i);
    }
    for(int i=0; i<=nVars; ++i) if(this->propagated[i]) delete this->propagated[i];
}


void CompleteEncodingSatProblemReducer::propagateAtDecisionLvlZero() {
    int nVars = this->initVarMap.nVars();
    std::vector<int> assumps;
    std::vector<int>& propagated = this->slv->propagatedAtDecisionLvlZero(assumps, true);
    std::vector<int> fixedTrue;
    std::vector<int> fixedFalse;
    for(unsigned int i=0; i<propagated.size(); ++i) {
        int lit = propagated[i];
        int var = lit > 0 ? lit : -lit;
        if(var > nVars) continue;
        this->fixed[var] = true;
        this->fixedVal[var] = lit > 0;
        if(lit > 0)
            fixedTrue.push_back(var);
        else
            fixedFalse.push_back(var);
    }
    for(unsigned int i=1; i<fixedTrue.size(); ++i) this->equivalentTo[fixedTrue[i]] = fixedTrue[0];
    for(unsigned int i=1; i<fixedFalse.size(); ++i) this->equivalentTo[fixedFalse[i]] = fixedFalse[0];
    if(fixedTrue.size()) this->eqClasses.push_back(fixedTrue);
    if(fixedFalse.size()) this->eqClasses.push_back(fixedFalse);
}


void CompleteEncodingSatProblemReducer::lookForEquivalencesOf(int var) {
    if(this->equivalentTo[var] != var) return;
    computePropagationsOf(var);
    std::vector<int> equivalencies;
    equivalencies.push_back(var);
    for(unsigned int i=0; i<this->propagated[var]->size(); ++i) {
        int prop = (*this->propagated[var])[i];
        if(prop <= var || prop > this->initVarMap.nVars() || this->fixed[prop] || this->equivalentTo[prop] != prop) continue;
        computePropagationsOf(prop);
        bool eq = false;
        for(unsigned int j=0; j<this->propagated[prop]->size(); ++j) {
            int locprop = (*this->propagated[prop])[j];
            if(locprop == var) {
                eq = true;
                break;
            } else if(locprop == -var) {
                break;
            }
        }
        if(eq) equivalencies.push_back(prop);
    }
    for(unsigned int i=1; i<equivalencies.size(); ++i) this->equivalentTo[equivalencies[i]] = var;
    this->eqClasses.push_back(equivalencies);
}


void CompleteEncodingSatProblemReducer::computePropagationsOf(int var) {
    if(this->propagated[var]) return;
    std::vector<int> assumps;
    assumps.push_back(var);
    std::vector<int>& propagated = this->slv->propagatedAtDecisionLvlZero(assumps);
    this->propagated[var] = new std::vector<int>;
    for(unsigned int i=0; i<propagated.size(); ++i) this->propagated[var]->push_back(propagated[i]);
}


void CompleteEncodingSatProblemReducer::translateVarMap() {
    this->reducedVarMap = std::make_shared<VarMap>();
    for(unsigned int i=0; i<eqClasses.size(); ++i) {
        std::vector<int> eqClass = eqClasses[i];
        std::vector<std::string> names;
        for(unsigned int j=0; j<eqClass.size(); ++j) {
            names.push_back(initVarMap.getName(eqClass[j]));
        }
        reducedVarMap->addEntry(names);
    }
}
