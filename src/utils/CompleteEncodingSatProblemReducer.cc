#include "CompleteEncodingSatProblemReducer.h"

using namespace CoQuiAAS;


CompleteEncodingSatProblemReducer::CompleteEncodingSatProblemReducer(VarMap& initVarMap, Attacks& initAttacks): SatProblemReducer(initVarMap, initAttacks) {}


void CompleteEncodingSatProblemReducer::init() {
    SatEncodingHelper helper(this->slv, this->initAttacks, this->initVarMap);
	int disjId0 = helper.reserveDisjunctionVars();
	helper.createAttackersDisjunctionVars(disjId0);
	helper.createCompleteEncodingConstraints(disjId0);
}
