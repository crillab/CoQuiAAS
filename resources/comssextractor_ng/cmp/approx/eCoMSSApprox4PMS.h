#ifndef eCoMSSApprox4PMS_h
#define eCoMSSApprox4PMS_h

#include "cmp/approx/coMSSApprox.h"

namespace CMP {
  class Enhanced_CoMSSApprox4PMS : public CoMSSApprox
  {

  public:
    Enhanced_CoMSSApprox4PMS(SatSolver* s, vec<Lit>& select, vector<int>& _map);
    void operator()(vec<Lit>& mss, vec<Lit>& coMss);

  };
}

#endif
