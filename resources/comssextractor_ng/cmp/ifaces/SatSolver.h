#ifndef SatSolver_h
#define SatSolver_h

#include <vector>

#include "cmp/ifaces/MetaSolver.h"
#include "cmp/utils/cmpTypes.h"

using namespace std;

namespace CMP
{
  class SatSolver : MetaSolver
  {
  protected:
    vec<lbool> m; //model of last sat call
    vector<lbool> model;
    //TODO update model after calling to solve()
    
    

  public:
  vec<Lit> strongAssums;
    SatSolver() {}
    virtual ~SatSolver() {}
    
    virtual bool solve2 () = 0;
    virtual bool solve2 (const vec<Lit>& assumps) = 0;
    virtual bool solve () = 0;
    virtual bool solve (Lit p) = 0;
    virtual bool solve (const vec<Lit>& assumps) = 0;
    virtual bool solve (const vec<Lit>& assumps, const vec<Lit>& mss) = 0;
    virtual bool solve (const int lim) = 0;

    virtual void initNbVariable(int n) = 0;
    virtual int nOriginalVars() = 0;
    virtual Var newVar(bool polarity = true, bool dvar = true) = 0;
    virtual bool addClause(const vec<Lit>& ps) = 0;

    virtual void addBlockingClause(const vec<Lit>& ps) = 0;
    
    virtual void uncheckedEnqueue(Lit p) = 0;
    virtual lbool value(Lit p) = 0;
    virtual lbool value(Var v) = 0;
    virtual lbool modelValue(Lit p) = 0;
    virtual lbool modelValue(Var v) = 0;
    virtual void setPolarity(Var v, bool b) = 0;

    vector<lbool>& getModel() {return model;}
    virtual void getModel(vec<lbool>& out) { m.copyTo(out); };
    virtual void getConflict(vec<Lit>& core) = 0;
    virtual int getPolarity(Var v) = 0;
    virtual double activity(Var v) = 0;
    virtual bool reason_is_undef(Var v) = 0;
    virtual void getTrail(vec<Lit>& trail) = 0;
    
    virtual int nAssigns() = 0;
    virtual int nClauses() = 0;
    virtual int nLearnts() = 0;
    virtual int nVars() = 0;


    virtual void allowLearnts(bool b) = 0;

    virtual int decisionLevel() = 0;
    virtual void newDecisionLevel() = 0;
    virtual bool propagate_() = 0;
    virtual Clause* propagate() = 0;
    virtual Clause* reason(Var x) = 0;
    virtual void analyzeFinal(Lit p, vec<Lit>& confl) = 0;
    virtual void cancelUntil(int level) = 0;
    virtual void restartUntil(int level) = 0;
    virtual void removeLearnts() = 0; //pruneLearnts
    virtual void removeFromTrail(Var v) = 0;
    //virtual void createNewClause(vec<CRef> &cls, vec<Lit> &lits, bool flag = true) = 0;
    virtual void addPhantomClause(vec<Lit>& ps) = 0;
    virtual void removePhantomClauses() = 0;

    //virtual CRef newClause(const vec<Lit>& ps);
    
    
    virtual void popLearnt() = 0;
    
    virtual bool originalVar(Var v) = 0;
    virtual Clause &getIth_clauses(int i) = 0;
    //virtual Clause& getIth_learnts(int i) = 0;
    
    virtual void interrupt() = 0;
    virtual void printStats() = 0;

    virtual int nCalls() = 0;

    virtual void printClauses() = 0;
  };
}
#endif
