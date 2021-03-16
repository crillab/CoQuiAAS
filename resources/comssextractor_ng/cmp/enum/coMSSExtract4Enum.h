#ifndef coMSSExtract4Enum_h
#define coMSSExtract4Enum_h

#include "cmp/core/coMSSExtract.h"
#include "cmp/enum/ModelRotator.h"

#define _print_ 0

using namespace std;
namespace CMP {

  
 struct TCMap {  //Transition Clause Map
   vector< vector<Lit> > tc_heap;
   vector<int> core_level;
   vector<unsigned int> n_tc;
 };


class Caching {
 private:
   SatSolverFactory sfact;
   SatSolver* slv;
   const vec<int>&  marked_s;
   vec< vec<Lit> > cores;
   vec<Lit> confl;
   
   vec< vector<Lit>* > crits_map;
   int coreIdx;
   
 public:

 Caching(int nvars, const vec<int>& marked): sfact(MINISAT), slv(&sfact.instance()), marked_s(marked) {slv->initNbVariable(nvars);}
   
   inline void store(vec<Lit>& confl) {cores.push(); confl.copyTo(cores.last()); crits_map.push(NULL);}

   inline void getConflict(vec<Lit>& core) {confl.copyTo(core);}

   inline void store(vector<Lit> crits) {crits_map.last() = new vector<Lit>(crits);}

   inline vector<Lit>* getCrits() {if(coreIdx != -1) return crits_map[coreIdx]; else return NULL;}

   inline void reset() {slv->cancelUntil(0); for(int i=0; i<cores.size(); i++) slv->addClause(cores[i]); cores.clear(); }
   
   inline bool hasSubSet(const vec<Lit>& assums) {
     confl.clear();
     coreIdx = -1;
     bool res = false;
     while(slv->decisionLevel() < assums.size()){
 	Lit next = assums[slv->decisionLevel()];
	if(slv->value(next) == l_False) { 
	  Clause* c = slv->reason(var(next));
	  if(c == NULL) confl.push(~next);
	  else {
	    coreIdx = c->index();
	    for(int i=0; i<c->size(); i++) {
	      if(marked_s[var((*c)[i])] || (var(next) == var((*c)[i]))) confl.push((*c)[i]); 
	      else{confl.clear(); slv->analyzeFinal(~next, confl); break;} 
	    }
	  }	  
	  res = true;
	  break;
	}
	slv->newDecisionLevel();
	if(slv->value(next) == l_True) continue;
	slv->uncheckedEnqueue(next);
	if(slv->decisionLevel() <= assums.size() - 1) {
	  Clause* c = slv->propagate();
	  if(c != NULL) {
	    assert(slv->decisionLevel() == assums.size()); 
	    slv->analyzeFinal(~next, confl);
	    res = true;
	    //break;
	  }
	}
     }
     slv->cancelUntil(assums.size()-1);
     return res;
   }//hasSubSet

};
 
class CoMSSExtract4Enum : public CoMSSExtract
{
 protected:
  ModelRotator* m_rottor;
  Caching* cache;
  bool rotation;
  bool caching; 
  TCMap tcMap;
  int dec_level;


  vec<int> marked_tc;
  vec<int> prime_imp;
  int stamp_tc;
  int stamp_imp;
  
 public:
#if 1  
  int nb_tc;
  int nb_cache_tc;
  int nb_cache;
  bool debug;
#endif
  
 CoMSSExtract4Enum(WCNF &f, bool mr=false, bool c=false): CoMSSExtract(f), m_rottor(NULL), cache(NULL),
    rotation(mr), caching(c), dec_level(0), stamp_tc(0), stamp_imp(1), nb_tc(0),nb_cache_tc(0), nb_cache(0), debug(false) {
    if(rotation) { m_rottor = new ModelRotator(f, orgSelector_map, freshSelector_map, mapOrgID2in);}
    if(caching) {cache = new Caching(slv->nVars(), markedSelector); }
    for(int i=0; i<slv->nVars(); i++) tcMap.core_level.push_back(0);
    for(int i=0; i<slv->nVars(); i++) marked_tc.push(0);
    for(int i=0; i<slv->nVars(); i++) prime_imp.push(0);
  }
  
  void search(vec<Lit> &mss, vec<Lit>& unknown, vec<Lit> &coMss) = 0;
  virtual void mappingTC(Lit clT, vec<Lit>& confl) = 0;  

  inline int getLevel() {return dec_level;}
  
  inline void reoderTC(const vector<Lit>& clsT, vec<Lit>& mss) {
    stamp_tc++;
    for(size_t i=0; i<clsT.size(); i++) marked_tc[var(clsT[i])] = stamp_tc;
   
    Lit l;
    int i,j;
    for(i = j = mss.size() - 1; i>=0; i--) {
      if(marked_tc[var(mss[i])] == stamp_tc) {
	l = mss[j];
	mss[j--] = mss[i];
	mss[i] = l;
      }
    }
  }//reorderTC

  
  inline void getUnexplored(vec<Lit>& mss, vec<Lit>& coMss)
  {
    //   printf("mss = ");printLits(mss);printf("coMSS : ");printLits(coMss);
    //prune visited branch	
    while(tcMap.tc_heap.size() && (tcMap.tc_heap.back().size() == 1)) {
      tcMap.tc_heap.pop_back();
      tcMap.n_tc.pop_back();
    }
    
    //update level
    int level = tcMap.tc_heap.size(); 
    for(size_t i=0; i<tcMap.core_level.size(); i++) if(tcMap.core_level[i]>level) tcMap.core_level[i] = 0;
    
    if(tcMap.tc_heap.size()) {
      Lit  p = tcMap.tc_heap.back().back();
      Lit q = tcMap.tc_heap.back().front();
      //      printf("p = ");printLit(p); printf("  q= ");printLit(q);printf("\n");

      int i,j;
      for(i = j = 0; i<mss.size(); i++) {
	if(((tcMap.core_level[var(mss[i])]!=0) &&
	   (tcMap.core_level[var(mss[i])] <= level) &&
	    (var(mss[i]) != var(p)) && (var(mss[i]) != var(q))) || (prime_imp[var(mss[i])] == stamp_imp))
	  mss[j++] = mss[i];
	if(var(mss[i]) == var(p)) dec_level = j;
      }    
      mss.shrink(i-j);
      mss.push(q);

      if(tcMap.n_tc.back() == tcMap.tc_heap.back().size()) {
	reoderTC(tcMap.tc_heap.back(), mss);
	dec_level = 0;
      }
      
      tcMap.tc_heap.back()[0] = p; //save Lit p for next mss
      tcMap.tc_heap.back().pop_back();
     
      while(var(coMss.last()) != var(q)) coMss.pop();
      coMss.last() = p;
    }
    else {
      dec_level = 0;
      coMss.clear(); mss.clear();
    }
  }//getUnexplored


  inline void initAssignment(vec<Lit>& mss) {
    stamp_imp++;
    mss.clear(); prime_imp.clear();
    assert(slv->getModel().size());
    for(int i=0; i<selectors.size(); i++) {
      if(slv->modelValue(selectors[i]) == l_True) {
	mss.push(selectors[i]);
	prime_imp[var(selectors[i])] = stamp_imp; 
      }
    }
  }//initAssignment
    
  /**
     Add the negation of the clause pointed by sel. 
     @param[in] sel, the selector
  */
  inline void backbone(Lit sel, vec<Lit> &assums)
  {
    if(slv->originalVar(var(sel))) return;
    Clause &c = slv->getIth_clauses(sel2cl(sel));;      
    for(int i = 0 ; i<c.size() ; i++)
      if((slv->value(c[i]) == l_Undef) && !markedSelector[var(c[i])])
	{
	  assums.push(~c[i]);
	  markedSelector[var(c[i])] = sign(~c[i]) + 1;
	}
  }// backbone

  inline void addClause(vec<Lit>& ps) {slv->addClause(ps);};
  inline ModelRotator* get_mrottor(void) {return m_rottor;}
  inline bool rotate(void) {return rotation;}
};

 
}//namespace

#endif
