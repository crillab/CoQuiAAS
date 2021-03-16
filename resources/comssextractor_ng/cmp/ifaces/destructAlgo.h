#ifndef destructAlgo_h
#define destructAlgo_h

#include <iostream>
#include <map>
#include "cmp/utils/Vec.h"
#include "cmp/utils/cmpTypes.h"

using namespace std;

namespace CMP {

class DestructAlgo {
 protected:
  Var phantomVar;

  virtual inline void clauseN(vec<Lit>& assumps) = 0;
  virtual void exploiteCore(vec<Lit>& assump, int n=1) = 0;
  bool extendMSS(vec<Lit>& assump, const vec<Lit>& unknown);
  bool extendMSS(vec<Lit>& assump, vec<Lit>& unknown, vec<Lit>& mss, vec<Lit>& prev_assump);
  virtual bool trimAssumps(vec<Lit> &assums, const vec<Lit>& unknown) = 0;

 
  inline void core2assump(const vec<Lit>& conflict, vec<Lit>& assumps)
  {
    assumps.clear();
    for(int i = 0 ; i<conflict.size() ; i++) assumps.push(~conflict[i]);
  }//core2assump

  /*return true, if u intersect v, else false*/
  inline bool intersect(const vec<Lit>& v1, const vec<Lit>& v2)
  {
    map<int,int> mapV1;
    for(int i=0; i<v1.size(); i++) mapV1[var(v1[i])] = 1;
    for(int i=0; i<v2.size(); i++) if(mapV1.find(var(v2[i])) != mapV1.end()) return true;
    return false;
  }//intersect

  /* inline void pickNextTC(vec<Lit>& assumps) */
  /* { */
  /*   int pos = 0; */
  /*   for(int i=1; i<assumps.size(); i++) if(slv->activity(var(assumps[pos]) < slv->activity(var(assumps[i]))) pos = i; */
  /*   //swap */
  /*   Lit p = assumps[pos]; */
  /*   assumps[pos] = assumps[0]; */
  /*   assumps[0] = p; */
  /* }//pickNextTC */

};

}//namespace

#endif
