#ifndef bCoMSSApprox_h
#define bCoMSSApprox_h

#include "cmp/approx/coMSSApprox.h"

namespace CMP {
  class Basic_CoMSSApprox : public CoMSSApprox
  {

  public:
    Basic_CoMSSApprox(SatSolver* s, vec<Lit>& select, vector<int>& _map);
    void operator()(vec<Lit>& mss, vec<Lit>& coMss);

  };
}//namespace

#endif
