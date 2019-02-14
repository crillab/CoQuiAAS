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
    /* std::cout << "eqClass: " << equivalencies[0];
    for(unsigned int i=1; i<equivalencies.size(); ++i) {
        this->equivalentTo[equivalencies[i]] = var;
        std::cout << " " << equivalencies[i];
    }
    std::cout << std::endl;*/
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
    /* for(unsigned int i=1; i<this->equivalentTo.size(); ++i) {
        if(this->equivalentTo[i] == (signed) i) {
            this->reducedVarMap->addEntry(this->initVarMap.getName(i));
        }
    } */
    for(unsigned int i=0; i<eqClasses.size(); ++i) {
        std::vector<int> eqClass = eqClasses[i];
        std::vector<std::string> names;
        for(unsigned int j=0; j<eqClass.size(); ++j) {
            names.push_back(initVarMap.getName(eqClass[j]));
        }
        reducedVarMap->addEntry(names);
    }
}


/* void CompleteEncodingSatProblemReducer::translateAttacks() {
    this->reducedAttacks = std::make_shared<Attacks>(*(this->reducedVarMap.get()));
    for(unsigned int i=0; i<this->initVarMap.nVars(); ++i) {
        int var = this->initVarMap.intVars()[i];
        if(this->fixed[var] && this->fixedVal[var]) continue;
        int newTo = this->equivalentTo[var];
        std::vector<int>* initAttacks = this->initAttacks.getAttacksTo(var);
        for(unsigned int j=0; j<initAttacks->size(); ++j) {
            int from = (*initAttacks)[j];
            int newFrom = this->equivalentTo[from];
            printf("adding attack %s -> %s\n", this->initVarMap.getName(newFrom).c_str(), this->initVarMap.getName(newTo).c_str());
            this->reducedAttacks->addAttack(this->initVarMap.getName(newFrom), this->initVarMap.getName(newTo));
        }
    }
    std::vector<std::tuple<bool, int, int, bool> >& dynAttacks = this->initAttacks.getDynAttacks();
    for(unsigned int i=0; i<dynAttacks.size(); ++i) {
        std::tuple<bool, int, int, bool> t = dynAttacks[i];
        int newFrom = this->equivalentTo[std::get<1>(t)];
        int newTo = this->equivalentTo[std::get<2>(t)];
        this->reducedAttacks->addDynAttack(std::get<0>(t), newFrom, newTo, std::get<3>(t));
    }
} */


/* std::vector<bool> CompleteEncodingSatProblemReducer::translateModel(std::vector<bool> reducedModel) {
    std::vector<bool> result(this->initVarMap.nVars());
    for(unsigned int i=0; i<this->initVarMap.nVars(); ++i) {
        string varName = this->initVarMap.getName(this->equivalentTo[this->initVarMap.intVars()[i]]);
        result[i] = reducedModel[this->reducedVarMap->getVar(varName)-1];
    }
    return result;
} */