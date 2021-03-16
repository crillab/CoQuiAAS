#ifndef __CF_SAT_PROBLEM_REDUCER_H__
#define __CF_SAT_PROBLEM_REDUCER_H__

#include "SatProblemReducer.h"

namespace CoQuiAAS {

class ConflictFreenessEncodingSatProblemReducer : public SatProblemReducer {

public:

    ConflictFreenessEncodingSatProblemReducer(VarMap& initVarMap, Attacks& initAttacks);

protected:

    void init();

};

}

#endif