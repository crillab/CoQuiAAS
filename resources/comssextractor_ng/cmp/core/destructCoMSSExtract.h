#ifndef destructCoMSSExtract_h
#define destructCoMSSExtract_h

#include "cmp/core/coMSSExtract.h"
#include "cmp/ifaces/destructAlgo.h"

using namespace std;

namespace CMP {

class DestructCoMSSExtract : public CoMSSExtract, DestructAlgo
{
 private:
  
  /*update the current coMSS */
  inline void shrinkCoMSS(vec<Lit> &coMss, bool assumsIsCoMss = false)
  {
    int i, j;
    slv->propagate();
    for(i = j = 0 ; i<coMss.size() ; i++) 
      {
	if(slv->value(coMss[i]) != l_Undef) continue;
	if(assumsIsCoMss || slv->modelValue(coMss[i]) == l_False) coMss[j++] = coMss[i];
	else slv->uncheckedEnqueue(coMss[i]);                
      }
    coMss.shrink(i - j);      
  }//shrinkCoMSS


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

  
  inline bool trimAssumps(vec<Lit> &approxCoMss, const vec<Lit>& unknown)
  {
    vec<Lit> saveFalse;
    int i, j;
    for(i = j = 0 ; i<approxCoMss.size() ; i++)
      {
	if(slv->value(approxCoMss[i]) == l_False) saveFalse.push(approxCoMss[i]);
	if(slv->value(approxCoMss[i]) != l_Undef) continue;
	if(slv->modelValue(approxCoMss[i]) == l_False) approxCoMss[j++] = approxCoMss[i];
      }
    approxCoMss.shrink(i - j);
    for(i = 0 ; i<unknown.size() ; i++) {
      if(slv->value(var(unknown[i])) == l_Undef && (slv->modelValue(unknown[i]) == l_True))
	slv->uncheckedEnqueue(unknown[i]);
    }
    if(saveFalse.size()) saveFalse.copyTo(approxCoMss);
    return saveFalse.size() != 0;
  }//trimAssumps

  inline void pickNextTC(vec<Lit>& assumps)
  {
    int pos = 0;
    for(int i=1; i<assumps.size(); i++) if(slv->activity(var(assumps[pos])) < slv->activity(var(assumps[i]))) pos = i;
    //swap
    Lit p = assumps[pos];
    assumps[pos] = assumps[0];
    assumps[0] = p;
  }//pickNextTC
    
  void exploiteCore(vec<Lit>& assump, int n);
  bool extendMSS(vec<Lit>& assump, const vec<Lit>& unknown);

 public:
  DestructCoMSSExtract(WCNF &f);
  void search(vec<Lit> &mss, vec<Lit>& unknown, vec<Lit> &coMss);    
};

}//namespace

#endif
