#include "StableEncodingSatProblemReducer.h"

using namespace CoQuiAAS;


StableEncodingSatProblemReducer::StableEncodingSatProblemReducer(VarMap& initVarMap, Attacks& initAttacks): SatProblemReducer(initVarMap, initAttacks) {}


void StableEncodingSatProblemReducer::init() {
    SatEncodingHelper helper(this->slv, this->initAttacks, this->initVarMap);
	helper.createStableEncodingConstraints();
}
