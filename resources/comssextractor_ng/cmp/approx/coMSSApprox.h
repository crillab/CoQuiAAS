#ifndef coMSSApprox_h
#define coMSSApprox_h

#include <vector>

#include "cmp/utils/Vec.h"
#include "cmp/utils/cmpTypes.h"
#include "cmp/ifaces/MetaSolver.h"
#include "cmp/ifaces/SatSolver.h"
//#include "cmp/approx/localSearch.h"

using namespace std;

namespace CMP {
  class CoMSSApprox : public MetaSolver
  {
  protected:
    SatSolver* slv;
    vec<Lit>& select;
    vector<int>& mapIdClsInSolver;
 
  public:
    unsigned int conf_lim;
    int local_search;

  CoMSSApprox(SatSolver* _s, vec<Lit>& _select, vector<int>& _map):
    slv(_s),  select(_select), mapIdClsInSolver(_map) {conf_lim = INT32_MAX; local_search=0;}
    
    virtual void operator()(vec<Lit>& mss, vec<Lit>& coMss) = 0;

    inline int sel2cl(Lit s) {return mapIdClsInSolver[var(s) - slv->nOriginalVars()];}

    inline void interrupt(){slv->interrupt();}    
    inline void printStats(){slv->printStats();}     
  };
}

#endif
