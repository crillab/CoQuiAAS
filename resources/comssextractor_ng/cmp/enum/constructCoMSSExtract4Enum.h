#ifndef constructCoMSSExtract4Enum_h
#define constructCoMSSExtract4Enum_h

#include "cmp/enum/coMSSExtract4Enum.h"
#include "cmp/enum/ModelRotator.h"

using namespace std;

namespace CMP
{  
  class ConstructCoMSSExtract4Enum : public CoMSSExtract4Enum
  {
    int stamp;
    vector<int> cand;

  public:
    ConstructCoMSSExtract4Enum(WCNF &f, bool mr=false, bool c=false);
    void search(vec<Lit> &mss, vec<Lit>& unknown, vec<Lit> &coMss);
    void mappingTC(Lit clT, vec<Lit>& confl);
    
    inline void addSingleTC(Lit clT) {
      vector<Lit> crits; 
      crits.push_back(clT);
      tcMap.tc_heap.push_back(crits);
      tcMap.n_tc.push_back(tcMap.tc_heap.back().size());
      if(!tcMap.core_level[var(clT)]) tcMap.core_level[var(clT)] = tcMap.tc_heap.size();
    }
    
  };
}

#endif
