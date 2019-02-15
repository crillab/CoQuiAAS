#include "ConflictFreenessEncodingSatProblemReducer.h"

using namespace CoQuiAAS;


ConflictFreenessEncodingSatProblemReducer::ConflictFreenessEncodingSatProblemReducer(VarMap& initVarMap, Attacks& initAttacks): SatProblemReducer(initVarMap, initAttacks) {}


void ConflictFreenessEncodingSatProblemReducer::init() {
    SatEncodingHelper helper(this->slv, this->initAttacks, this->initVarMap);
    int disjId = helper.reserveDisjunctionVars();
	helper.createAttackersDisjunctionVars(disjId);
	helper.createConflictFreenessEncodingConstraints(disjId);
}
