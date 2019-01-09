#ifndef GlucoseSolver_h
#define GlucoseSolver_h

#include "glucose/core/Solver.h"

using namespace CMP;

class GlucoseSolver : public SatSolver
{
 protected :
  Glucose::Solver* slv;

  //TODO update model after calling to solve()
  
  //  friend class SatSolverFactory;
  
 public:
  GlucoseSolver() {slv = new Glucose::Solver();}
  ~GlucoseSolver() {if(!slv) {delete slv; slv = NULL;}}


  bool solve2 () 
  {
    vec<Lit> emptyVec;
    return solve2(emptyVec);
  }
  bool solve2 (const vec<Lit>& assumps) {
    Glucose::vec<Glucose::Lit> m_assumps;
    for(int i = 0 ; i<strongAssums.size() ; i++) m_assumps.push(Glucose::mkLit((int)var(strongAssums[i]), sign(strongAssums[i])));
    for(int i=0; i<assumps.size(); i++) m_assumps.push(Glucose::mkLit((int)var(assumps[i]), sign(assumps[i])));
    return slv->solve(m_assumps);
  }
  bool solve () 
  {
    vec<Lit> emptyVec;
    return solve(emptyVec);
  }
  bool solve (Lit p) 
  {
    vec<Lit> notEmptyVec;
    notEmptyVec.push(p);
    return solve(notEmptyVec);
  }
  bool solve (const vec<Lit>& assumps) {
    Glucose::vec<Glucose::Lit> m_assumps;
    for(int i = 0 ; i<strongAssums.size() ; i++) m_assumps.push(Glucose::mkLit((int)var(strongAssums[i]), sign(strongAssums[i])));
    for(int i=0; i<assumps.size(); i++) m_assumps.push(Glucose::mkLit((int)var(assumps[i]), sign(assumps[i])));
    return slv->solve(m_assumps);
  }
  bool solve (const vec<Lit>& assumps, const vec<Lit>& mss) {
    //TODO
    Glucose::vec<Glucose::Lit> m_assumps, m_mss;
    for(int i = 0 ; i<strongAssums.size() ; i++) m_assumps.push(Glucose::mkLit((int)var(strongAssums[i]), sign(strongAssums[i])));
    for(int i=0; i<assumps.size(); i++) m_assumps.push(Glucose::mkLit((int)var(assumps[i]), sign(assumps[i])));
    for(int i=0; i<mss.size(); i++) m_mss.push(Glucose::mkLit((int)var(mss[i]), sign(mss[i])));
    return slv->solve(m_assumps, m_mss);
  }
  bool solve (const int lim)
  { 
      assert(0);    
     return slv->solve(lim);
  }

  void initNbVariable(int n) { slv->initNbInitialVars(n);}
  Var newVar(bool polarity = true, bool dvar = true) {return slv->newVar(polarity, dvar);}
  bool addClause(const vec<Lit>& ps) {
    Glucose::vec<Glucose::Lit> m_lits;
    for(int i=0; i<ps.size(); i++) m_lits.push(Glucose::mkLit((int)var(ps[i]), sign(ps[i])));
    return slv->addClause(m_lits);
  }
      
  void uncheckedEnqueue(Lit p) {slv->uncheckedEnqueue(Glucose::mkLit((int)var(p), sign(p)));}

  lbool value(Lit p) {
    Glucose::lbool m_val = slv->value(Glucose::mkLit((int)var(p), sign(p)));
    lbool val = (Glucose::toInt(m_val) == 0)? l_True : ((Glucose::toInt(m_val) == 1)? l_False : l_Undef);
    return val;
  }

  lbool value(Var v) {
    Glucose::lbool m_val = slv->value((int)v);
    lbool val = (Glucose::toInt(m_val) == 0)? l_True : ((Glucose::toInt(m_val) == 1)? l_False : l_Undef);
    return val;
  }

  lbool modelValue(Lit p) {
    assert(slv->model.size()>var(p)); 
    Glucose::lbool m_val = slv->modelValue(Glucose::mkLit((int)var(p), sign(p)));
    lbool val = (Glucose::toInt(m_val) == 0)? l_True : ((Glucose::toInt(m_val) == 1)? l_False : l_Undef);
    return val;
  }

  lbool modelValue(Var v) {
    assert(slv->model.size() > v); 
    Glucose::lbool m_val = slv->modelValue(v);
    lbool val = (Glucose::toInt(m_val) == 0)? l_True : ((Glucose::toInt(m_val) == 1)? l_False : l_Undef);
    return val;
  }

  void setPolarity(Var v, bool b) {slv->setPolarity(v,b);}

  void getConflict(vec<Lit>& core) {
    for(int i=0; i<slv->conflict.size(); i++) core.push(mkLit((int)Glucose::var(slv->conflict[i]), Glucose::sign(slv->conflict[i])));
  }

  int getPolarity(Var v) {return slv->polarity[v];}

  double activity(Var v) {return slv->activity[(int)v];}

  bool reason_is_undef(Var v) {return (slv->reason((int)v) == Glucose::CRef_Undef);}
  
  void getTrail(vec<Lit>& trail) {trail.clear();}//TODO

  int nAssigns() {return slv->nAssigns();}
  int nClauses() {return slv->nClauses();}
  int nLearnts() {return slv->nLearnts();}
  int nVars() {return slv->nVars();}


  void allowLearnts(bool b) {slv->canTouchLearnt = b;}

  int decisionLevel() {return slv->decisionLevel();}
  void newDecisionLevel() {slv->newDecisionLevel();}
  bool propagate_() {return (slv->propagate() == Glucose::CRef_Undef);}
  void cancelUntil(int level) {slv->cancelUntil(level);}
  void restartUntil(int level) {slv->restartUntil(level);}
  void removeLearnts() {slv->removeLearnts();}
  void removeFromTrail(Var v) {slv->removeFromTrail(v);}

  void analyzeFinal(Lit p, vec<Lit>& confl) {
    Glucose::Lit m_lit = Glucose::mkLit((int)var(p), sign(p)); 
    Glucose::vec<Glucose::Lit> out_conflict;
    slv->analyzeFinal(m_lit, out_conflict);
    confl.clear();
    for(int i=0; i<out_conflict.size(); i++) confl.push(mkLit((int)Glucose::var(out_conflict[i]), Glucose::sign(out_conflict[i])));
  }

  Clause* propagate() {
    //Glucose::CRef cr = slv->propagate();
    return NULL;
    //TODO
    /* if(cr == Glucose::CRef_Undef) return NULL; */
    /* return getIth_clauses(slv->ca[cr].index()); */
  }

  Clause* reason(Var x) {
    printf("%d\n", x);
    /* Minisat::CRef cr = slv->reason(x); */
    return NULL;
    /* if(cr == Minisat::CRef_Undef) return NULL; */
    /* return &(getIth_clauses(slv->ca[cr].index())); */
  }
    
  void addPhantomClause(vec<Lit>& ps) {
    //TODO
    Glucose::vec<Glucose::Lit> m_lits;
    for(int i=0; i<ps.size(); i++) m_lits.push(Glucose::mkLit((int)var(ps[i]), sign(ps[i])));
    slv->addPhantomClause(m_lits);
  }
  void removePhantomClauses() {slv->removePhantomClauses();}

  void popLearnt() {slv->removeClause((slv->learnts).last()); slv->learnts.pop();}//TODO
    
  bool originalVar(Var v) {return slv->originalVar((int)v);} //TODO

  Clause a;
  Clause &getIth_clauses(int i) {
    assert(i<nClauses() && 0);
    Glucose::Clause& c = slv->ca[slv->clauses[i]];    
    //Lit lits[c.size()];
    vec<Lit> lits;
    for(int j=0; j<c.size(); j++) lits.push(mkLit((int)Glucose::var(c[j]), Glucose::sign(c[j])));
    a = Clause(lits,  i);
    return a;
  }

  int nOriginalVars() {return slv->nbVarsInitialFormula;}

  void addBlockingClause(const vec<Lit>& ps)
  {
    //TODO
    vec<Lit> lits; ps.copyTo(lits);
    addClause(lits);
  }

  int nCalls() {return 0;}//TODO
  
  void printStats() {}
  void interrupt() {}

  void printClauses() { } //TODO
};

#endif
