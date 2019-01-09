#ifndef MiniSatSolver_h
#define MiniSatSolver_h

#include "minisat/core/Solver.h"

using namespace CMP;

class MiniSatSolver : public SatSolver
{
 protected :
  // Minisat::Solver* slv;
  vec<Clause> clauses;
  
  //  friend class SatSolverFactory;

 public:
  MiniSatSolver(){slv = new Minisat::Solver();}
  ~MiniSatSolver() {if(!slv) {delete slv; slv = NULL;}}
  Minisat::Solver* slv;

  bool solve2 () {
    CMP::vec<CMP::Lit> m_assumps;    
    return solve2(m_assumps);          
  }

  bool solve2 (const vec<Lit>& assumps) {
    Minisat::vec<Minisat::Lit> m_assumps;    
    for(int i = 0 ; i<strongAssums.size() ; i++) m_assumps.push(Minisat::mkLit((int)var(strongAssums[i]), sign(strongAssums[i])));
    for(int i = 0 ; i<assumps.size() ; i++) m_assumps.push(Minisat::mkLit((int)var(assumps[i]), sign(assumps[i])));    
  
    slv->deact_selector = true;
    bool res = slv->solve(m_assumps);
    slv->deact_selector = false;
    if(res) {model.clear(); for(int i=0; i<nVars(); i++) model.push_back(modelValue(i)); }
    return res;
  }

  ///
  bool solve () {
    CMP::vec<CMP::Lit> m_assumps;    
    return solve(m_assumps);        
  }

  bool solve (Lit p) {
    CMP::vec<CMP::Lit> m_assumps;
    m_assumps.push(p);
    return solve(m_assumps);    
  }

  bool solve (const vec<Lit>& assumps) {
    Minisat::vec<Minisat::Lit> m_assumps;    
    for(int i = 0 ; i<strongAssums.size() ; i++) m_assumps.push(Minisat::mkLit((int)var(strongAssums[i]), sign(strongAssums[i])));
    for(int i = 0 ; i<assumps.size() ; i++) m_assumps.push(Minisat::mkLit((int)var(assumps[i]), sign(assumps[i])));    
    bool res = slv->solve(m_assumps);
    if(res) {model.clear(); for(int i=0; i<nVars(); i++) model.push_back(modelValue(i)); }
    return res;
  }

  bool solve (const vec<Lit>& assumps, const vec<Lit>& mss) {
    //TODO
    Minisat::vec<Minisat::Lit> m_assumps, m_mss;
    for(int i = 0 ; i<strongAssums.size() ; i++) m_assumps.push(Minisat::mkLit((int)var(strongAssums[i]), sign(strongAssums[i])));
    for(int i = 0 ; i<assumps.size() ; i++) m_assumps.push(Minisat::mkLit((int)var(assumps[i]), sign(assumps[i])));    
        
    clone(mss, m_mss);
    bool res = slv->solve(m_assumps, m_mss);
    if(res) {model.clear(); for(int i=0; i<nVars(); i++) model.push_back(modelValue(i)); }
    return res;
  }

  bool solve (const int lim) {
    assert(0);
    bool res = slv->solve(lim);
    if(res) {model.clear(); for(int i=0; i<nVars(); i++) model.push_back(modelValue(i)); }
    return res;
  }

  void initNbVariable(int n) { slv->initNbVariable(n);}
  Var newVar(bool polarity = true, bool dvar = true) {return slv->newVar(polarity, dvar);}
  bool addClause(const vec<Lit>& ps)
  {
    assert(clauses.size() == nClauses());
    Minisat::vec<Minisat::Lit> m_lits;
    clone(ps, m_lits);
    int prev_nb = nClauses();
    bool res = slv->addClause_(m_lits);
    if(nClauses() == prev_nb+1) {
      vec<Lit> lits; clone(m_lits, lits);
      clauses.push(); clauses.last().init(lits, nClauses() - 1);
    }
    return res;
  }// addClause
      
  void uncheckedEnqueue(Lit p) {slv->uncheckedEnqueue(Minisat::mkLit((int)var(p), sign(p)));}

  lbool value(Lit p) {
    Minisat::lbool m_val = slv->value(Minisat::mkLit((int)var(p), sign(p)));
    lbool val = (Minisat::toInt(m_val) == 0)? l_True : ((Minisat::toInt(m_val) == 1)? l_False : l_Undef);
    return val;
  }

  lbool value(Var v) {
    Minisat::lbool m_val = slv->value((int)v);
    lbool val = (Minisat::toInt(m_val) == 0)? l_True : ((Minisat::toInt(m_val) == 1)? l_False : l_Undef);
    return val;
  }

  lbool modelValue(Lit p) {
    assert(slv->model.size()>var(p)); 
    Minisat::lbool m_val = slv->modelValue(Minisat::mkLit((int)var(p), sign(p)));
    lbool val = (Minisat::toInt(m_val) == 0)? l_True : ((Minisat::toInt(m_val) == 1)? l_False : l_Undef);
    return val;
  }

  lbool modelValue(Var v) {
    assert(slv->model.size() > v); 
    Minisat::lbool m_val = slv->modelValue(v);
    lbool val = (Minisat::toInt(m_val) == 0)? l_True : ((Minisat::toInt(m_val) == 1)? l_False : l_Undef);
    return val;
  }

  void setPolarity(Var v, bool b) {slv->setPolarity(v,b);}

  void getConflict(vec<Lit>& core) {
    clone(slv->conflict, core);
  }

  int getPolarity(Var v) {return slv->polarity[v];}

  double activity(Var v) {return slv->activity[(int)v];}

  bool reason_is_undef(Var v) {return (slv->reason((int)v) == Minisat::CRef_Undef);}

  void getTrail(vec<Lit>& trail) {clone(slv->trail, trail);}  

  int nAssigns() {return slv->nAssigns();}
  int nClauses() {return slv->nClauses();}
  int nLearnts() {return slv->nLearnts();}
  int nVars() {return slv->nVars();}


  void allowLearnts(bool b) {slv->canTouchLearnt = b;}

  int decisionLevel() {return slv->decisionLevel();}
  void newDecisionLevel() {slv->newDecisionLevel();}
  bool propagate_() {return (slv->propagate() == Minisat::CRef_Undef);}
  void cancelUntil(int level) {slv->cancelUntil(level);}
  void restartUntil(int level) {slv->restartUntil(level);}
  void removeLearnts() {slv->removeLearnts();}
  void removeFromTrail(Var v) {slv->removeFromTrail(v);}

  void analyzeFinal(Lit p, vec<Lit>& confl) {
    Minisat::Lit m_lit = Minisat::mkLit((int)var(p), sign(p)); 
    Minisat::vec<Minisat::Lit> out_conflict;
    slv->analyzeFinal(m_lit, out_conflict);
    clone(out_conflict, confl);
  }
  
  Clause* propagate() {
    Minisat::CRef cr = slv->propagate();
    if(cr == Minisat::CRef_Undef) return NULL;
    return &(getIth_clauses(slv->ca[cr].index()));
  }

  Clause* reason(Var x) {
    Minisat::CRef cr = slv->reason((int)x);
    if(cr == Minisat::CRef_Undef) return NULL;
    return &(getIth_clauses(slv->ca[cr].index()));
  }
  
  void addPhantomClause(vec<Lit>& ps) {
    //TODO
    Minisat::vec<Minisat::Lit> m_lits;
    for(int i=0; i<ps.size(); i++) m_lits.push(Minisat::mkLit((int)var(ps[i]), sign(ps[i])));
    slv->addPhantomClause(m_lits);
  }
  void removePhantomClauses() {slv->removePhantomClauses();}

  /* void newClause(const vec<Lit>& ps) { */
  /*   Minisat::CRef m_cr = (slv->ca).alloc(ps, false); */
  /* } */
    
  //void deleteClause(int index) { }//TODO
  void popLearnt() {slv->removeClause((slv->learnts).last()); slv->learnts.pop();}//TODO
    
  bool originalVar(Var v) {return slv->originalVar((int)v);} //TODO

  Clause &getIth_clauses(int i) {
    assert(i<nClauses());

#if 0
    Minisat::Clause& c = slv->ca[slv->clauses[i]];
    vec<Lit> lits;
    for(int j=0; j<c.size(); j++) lits.push(mkLit((int)Minisat::var(c[j]), Minisat::sign(c[j])));
    return Clause(lits ,i);
#else
    assert(i<clauses.size() && clauses[i].index() == i);
    return clauses[i];
#endif
  }

  // Clause& getIth_learnts(int i) {assert(i<nLearnts()); return slv->ca[learntss[i]];}

  int nOriginalVars() {return slv->nbInitVariable;}

  void addBlockingClause(const vec<Lit>& ps)
  {
    //TODO
    vec<Lit> lits; ps.copyTo(lits);
    addClause(lits);
  }

  
  void printStats() {slv->printStats();}
  void interrupt() {slv->interrupt();}

  int nCalls() {return slv->nbSATCall+slv->nbUnsatCall;}

  inline void clone(const vec<Lit>& in, Minisat::vec<Minisat::Lit>& out) {
    out.clear();
    for(int i=0; i<in.size(); i++) out.push(Minisat::mkLit((int)var(in[i]), sign(in[i])));
  }
  
  inline void clone(const Minisat::vec<Minisat::Lit>& in, vec<Lit>& out) {
    out.clear();
    for(int i=0; i<in.size(); i++) out.push(mkLit((int)Minisat::var(in[i]), Minisat::sign(in[i])));
  }

  void printClauses() {slv->printAllClauses();}

};

#endif
