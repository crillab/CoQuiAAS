#ifndef bCoMSSApprox4PMS_h
#define bCoMSSApprox4PMS_h

#include "cmp/approx/coMSSApprox.h"

namespace CMP {
  class Basic_CoMSSApprox4PMS : public CoMSSApprox
  {

  public:
    Basic_CoMSSApprox4PMS(SatSolver* s, vec<Lit>& select, vector<int>& _map);
    void operator()(vec<Lit>& mss, vec<Lit>& coMss);

  };
}//namespace

#endif
