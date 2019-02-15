#ifndef __CO_SAT_PROBLEM_REDUCER_H__
#define __CO_SAT_PROBLEM_REDUCER_H__

#include "SatProblemReducer.h"

namespace CoQuiAAS {

class CompleteEncodingSatProblemReducer : public SatProblemReducer {

public:

    CompleteEncodingSatProblemReducer(VarMap& initVarMap, Attacks& initAttacks);

protected:

    void init();

};

}

#endif