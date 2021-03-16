#ifndef destructCoMSSExtract4Enum_h
#define destructCoMSSExtract4Enum_h

#include "cmp/enum/coMSSExtract4Enum.h"
#include "cmp/ifaces/destructAlgo.h"

using namespace std;

namespace CMP
{
  class DestructCoMSSExtract4Enum : public CoMSSExtract4Enum, DestructAlgo
  {
  private:

    inline void core2assump(const vec<Lit>& conflict, vec<Lit>& assumps)
    {
      //slv->printLits(assumps); slv->printLits(slv->conflict);
      vec<Lit> core;
      map<int,int> mapAssumps;
      for(int i=0; i<assumps.size(); i++) mapAssumps[var(assumps[i])] = 1;
      for(int i=0; i<conflict.size(); i++) if(mapAssumps.find(var(conflict[i])) != mapAssumps.end()) core.push(conflict[i]);
      assumps.clear();
      for(int i = 0 ; i<core.size() ; i++) assumps.push(~core[i]);
    }//core2assump
      
    /*add to assumption the negation of clause having selecotr assums[0]*/
    inline void clauseN(vec<Lit>& assumps)
    {
      assert(assumps.size());
      if(assumps.size() == 1) return;
      Lit s = assumps[0];
      if(slv->originalVar(var(s))) return;
      //assert(mapIdxClauseInSolver.find(toInt(~s)) != mapIdxClauseInSolver.end());

      vec<Lit> newAssumps;
      newAssumps.push(s);
      //int i = mapIdxClauseInSolver[toInt(~s)];
      Clause& c = slv->getIth_clauses(sel2cl(s));
      for(int i = 0; i<c.size(); i++) if((var(s) != var(c[i])) && (slv->value(c[i])==l_Undef)) newAssumps.push(~c[i]);
      for(int i = 1; i<assumps.size(); i++) newAssumps.push(assumps[i]);
      newAssumps.copyTo(assumps);
    }//clauseN
  
    inline bool trimAssumps(vec<Lit> &assums, const vec<Lit>& unknown)
    {
      vec<Lit> saveFalse;
      int i, j;
      for(i = j = 0 ; i<assums.size() ; i++)
	{
	  if(slv->value(assums[i]) == l_False) saveFalse.push(assums[i]);
	  if(slv->value(assums[i]) != l_Undef) continue;
	  if(slv->modelValue(assums[i]) == l_False) assums[j++] = assums[i];
	}
      assums.shrink(i - j);
        
      for(i = 0; i<unknown.size(); i++) {
	if(slv->modelValue(unknown[i]) == l_True) markedSelector[var(unknown[i])] = sign(unknown[i]) + 1;
      }
      if(saveFalse.size()) saveFalse.copyTo(assums);
      return saveFalse.size() != 0;
    }//trimAssumps
    
    void exploiteCore(vec<Lit>& assump, int n);
    bool extendMSS(vec<Lit>& assump, vec<Lit>& unknown, vec<Lit>& mss, vec<Lit>& prev_assump);
      
  public:
    DestructCoMSSExtract4Enum(WCNF &f);
    void search(vec<Lit> &mss, vec<Lit>& unknown, vec<Lit> &coMss);
    inline void mappingTC(Lit clT, vec<Lit>& confl){
      printLit(clT);
      printLits(confl);
      //TODO
    } 
  };
}

#endif
