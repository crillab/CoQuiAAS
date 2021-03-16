#ifndef __ST_SAT_PROBLEM_REDUCER_H__
#define __ST_SAT_PROBLEM_REDUCER_H__

#include "SatProblemReducer.h"

namespace CoQuiAAS {

class StableEncodingSatProblemReducer : public SatProblemReducer {

public:

    StableEncodingSatProblemReducer(VarMap& initVarMap, Attacks& initAttacks);

protected:

    void init();

};

}

#endif