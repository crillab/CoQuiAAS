#ifndef eCoMSSApprox_h
#define eCoMSSApprox_h

#include "cmp/approx/coMSSApprox.h"

namespace CMP {
  class Enhanced_CoMSSApprox : public CoMSSApprox
  {

  public:
    Enhanced_CoMSSApprox(SatSolver* s, vec<Lit>& select, vector<int>& _map);
    void operator()(vec<Lit>& mss, vec<Lit>& coMss);

  };
}

#endif
