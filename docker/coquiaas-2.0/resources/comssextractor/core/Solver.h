/****************************************************************************************[Solver.h]
Copyright (c) 2003-2006, Niklas Een, Niklas Sorensson
Copyright (c) 2007-2010, Niklas Sorensson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************************/

#define PRINT_DEBUG 0

#ifndef Minisat_Solver_h
#define Minisat_Solver_h

#include "utils/System.h"
#include "mtl/Vec.h"
#include "mtl/Heap.h"
#include "mtl/Alg.h"
#include "mtl/Sort.h"
#include "utils/Options.h"
#include "core/SolverTypes.h"
#include "core/SolverRL.h"


namespace Minisat {

//=================================================================================================
// Solver -- the main class:

#define SELECT_IS_DEC 0

struct coreWrtLit
{
  Lit l;
  vec<Lit> core;
};

typedef struct
{
  Lit l;
  int index;
} litUnit;

typedef struct
{
  Lit selector;
  int index;
  int posInClauses;
} infoSelector;

class Solver {
public:

    // Constructor/Destructor:
    //
    Solver();
    virtual ~Solver();

    // Problem specification:
    //
    Var     newVar    (bool polarity = true, bool dvar = true); // Add a new variable with parameters specifying variable mode.

    bool    addClause (const vec<Lit>& ps);                     // Add a clause to the solver. 
    bool    addEmptyClause();                                   // Add the empty clause, making the solver contradictory.
    bool    addClause (Lit p);                                  // Add a unit clause to the solver. 
    bool    addClause (Lit p, Lit q);                           // Add a binary clause to the solver. 
    bool    addClause (Lit p, Lit q, Lit r);                    // Add a ternary clause to the solver. 
    bool    addClause_(vec<Lit>& ps, bool isHard = false);      // Add a clause to the solver without making 
                                                                // superflous internal copy. Will
                                                                // change the passed vector 'ps'.

    // Solving:
    //
    bool    simplify     ();                        // Removes already satisfied clauses.
    bool solve (const vec<Lit>& a1, const vec<Lit>& a2);
    bool    solve        (const vec<Lit>& assumps); // Search for a model that respects a given set of assumptions.
    lbool   solveLimited (const vec<Lit>& assumps); // Search for a model that respects a given set of assumptions (With resource constraints).
    bool    solve        ();                        // Search without assumptions.
    bool    solve        (Lit p);                   // Search for a model that respects a single assumption.
    bool    solve        (Lit p, Lit q);            // Search for a model that respects two assumptions.
    bool    solve        (Lit p, Lit q, Lit r);     // Search for a model that respects three assumptions.
    bool    okay         () const;                  // FALSE means solver is in a conflicting state

    void    toDimacs     (FILE* f, const vec<Lit>& assumps);            // Write CNF to file in DIMACS-format.
    void    toDimacs     (const char *file, const vec<Lit>& assumps);
    void    toDimacs     (FILE* f, Clause& c, vec<Var>& map, Var& max);

    // Convenience versions of 'toDimacs()':
    void    toDimacs     (const char* file);
    void    toDimacs     (const char* file, Lit p);
    void    toDimacs     (const char* file, Lit p, Lit q);
    void    toDimacs     (const char* file, Lit p, Lit q, Lit r);
    
    // Variable mode:
    // 
    void    setPolarity    (Var v, bool b); // Declare which polarity the decision heuristic should use for a variable. Requires mode 'polarity_user'.
    void    setDecisionVar (Var v, bool b); // Declare if a variable should be eligible for selection in the decision heuristic.

    // Read state:
    //
    lbool   value      (Var x) const;       // The current value of a variable.
    lbool   value      (Lit p) const;       // The current value of a literal.
    lbool   modelValue (Var x) const;       // The value of a variable in the last model. The last call to solve must have been satisfiable.
    lbool   modelValue (Lit p) const;       // The value of a literal in the last model. The last call to solve must have been satisfiable.
    int     nAssigns   ()      const;       // The current number of assigned literals.
    int     nClauses   ()      const;       // The current number of original clauses.
    int     nLearnts   ()      const;       // The current number of learnt clauses.
    int     nVars      ()      const;       // The current number of variables.
    int     nFreeVars  ()      const;

    // Resource contraints:
    //
    void    setConfBudget(int64_t x);
    void    setPropBudget(int64_t x);
    void    budgetOff();
    void    interrupt();          // Trigger a (potentially asynchronous) interruption of the solver.
    void    clearInterrupt();     // Clear interrupt indicator flag.

    // Memory managment:
    //
    virtual void garbageCollect();
    void    checkGarbage(double gf);
    void    checkGarbage();

    // Extra results: (read-only member variable)
    //
    vec<lbool> model;             // If problem is satisfiable, this vector contains the model (if any).
    vec<Lit>   conflict;          // If problem is unsatisfiable (possibly under assumptions),
                                  // this vector represent the final conflict clause expressed in the assumptions.

    // Mode of operation:
    //
    int       verbosity;
    double    var_decay;
    double    clause_decay;
    double    random_var_freq;
    double    random_seed;
    bool      luby_restart;
    int       ccmin_mode;         // Controls conflict clause minimization (0=none, 1=basic, 2=deep).
    int       phase_saving;       // Controls the level of phase saving (0=none, 1=limited, 2=full).
    bool      rnd_pol;            // Use random polarities for branching heuristics.
    bool      rnd_init_act;       // Initialize variable activities with a small random value.
    double    garbage_frac;       // The fraction of wasted memory allowed before a garbage collection is triggered.

    int       restart_first;      // The initial restart limit.                                                                (default 100)
    double    restart_inc;        // The factor with which the restart limit is multiplied in each restart.                    (default 1.5)
    double    learntsize_factor;  // The intitial limit for learnt clauses is a factor of the original clauses.                (default 1 / 3)
    double    learntsize_inc;     // The limit for learnt clauses is multiplied with this factor each restart.                 (default 1.1)

    int       learntsize_adjust_start_confl;
    double    learntsize_adjust_inc;

    // Statistics: (read-only member variable)
    //
    uint64_t solves, starts, decisions, rnd_decisions, propagations, conflicts;
    uint64_t dec_vars, clauses_literals, learnts_literals, max_literals, tot_literals;



    // Helper structures:
    //
    struct VarData { CRef reason; int level; };
    static inline VarData mkVarData(CRef cr, int l){ VarData d = {cr, l}; return d; }

    struct Watcher {
        CRef cref;
        Lit  blocker;
        Watcher(CRef cr, Lit p) : cref(cr), blocker(p) {}
        bool operator==(const Watcher& w) const { return cref == w.cref; }
        bool operator!=(const Watcher& w) const { return cref != w.cref; }
    };

    struct WatcherDeleted
    {
        const ClauseAllocator& ca;
        WatcherDeleted(const ClauseAllocator& _ca) : ca(_ca) {}
        bool operator()(const Watcher& w) const { return ca[w.cref].mark() == 1; }
    };

    struct VarOrderLt {
        const vec<double>&  activity;
        bool operator () (Var x, Var y) const { return activity[x] > activity[y]; }
        VarOrderLt(const vec<double>&  act) : activity(act) { }
    };

    // Solver state:
    //
    bool                ok;               // If FALSE, the constraints are already unsatisfiable. No part of the solver state may be used!
    vec<CRef>           clauses;          // List of problem clauses
    vec<CRef>           blockedClauses;   // List of learnt clauses
    vec<CRef>           learnts;          // List of learnt clauses.
    double              cla_inc;          // Amount to bump next clause with.
    vec<double>         activity;         // A heuristic measurement of the activity of a variable.
    double              var_inc;          // Amount to bump next variable with.
    OccLists<Lit, vec<Watcher>, WatcherDeleted>
                        watches;          // 'watches[lit]' is a list of constraints watching 'lit' (will go there if literal becomes true).
    vec<lbool>          assigns;          // The current assignments.
    vec<char>           polarity;         // The preferred polarity of each variable.
    vec<char>           decision;         // Declares if a variable is eligible for selection in the decision heuristic.
    vec<Lit>            trail;          // Assignment stack; stores all assigments made in the order they were made.
    vec<int>            trail_lim;      // Separator indices for different decision levels in 'trail'.
    vec<VarData>        vardata;        // Stores reason and level for each variable.
    int                 qhead;          // Head of queue (as index into the trail -- no more explicit propagation queue in MiniSat).
    int                 simpDB_assigns;   // Number of top-level assignments since last execution of 'simplify()'.
    int64_t             simpDB_props;     // Remaining number of propagations that must be made before next execution of 'simplify()'.
    vec<Lit>            assumptions;      // Current set of assumptions provided to solve by the user.
    Heap<VarOrderLt>    order_heap;       // A priority queue of variables ordered with respect to the variable activity.
    double              progress_estimate;// Set by 'search()'.
    bool                remove_satisfied; // Indicates whether possibly inefficient linear scan for satisfied clauses should be performed in 'simplify'.

    ClauseAllocator     ca;

    // Temporaries (to reduce allocation overhead). Each variable is prefixed by the method in which it is
    // used, exept 'seen' wich is used in several places.
    //
    vec<char>           seen;
    vec<Lit>            analyze_stack;
    vec<Lit>            analyze_toclear;
    vec<Lit>            add_tmp;

    double              max_learnts;
    double              learntsize_adjust_confl;
    int                 learntsize_adjust_cnt;
    bool canTouchLearnt;

    // Resource contraints:
    //
    int64_t             conflict_budget;    // -1 means no budget.
    int64_t             propagation_budget; // -1 means no budget.
    bool                asynch_interrupt;

    // Main internal methods:
    //
    void     insertVarOrder   (Var x);                                                 // Insert a variable in the decision order priority queue.
    Lit      pickBranchLit    ();                                                      // Return the next decision variable.
    void     newDecisionLevel ();                                                      // Begins a new decision level.
    void     uncheckedEnqueue (Lit p, CRef from = CRef_Undef);                         // Enqueue a literal. Assumes value of literal is undefined.
    bool     enqueue          (Lit p, CRef from = CRef_Undef);                         // Test if fact 'p' contradicts current state, enqueue otherwise.
    CRef     propagate        ();                                                      // Perform unit propagation. Returns possibly conflicting clause.
    void     cancelUntil      (int level);                                             // Backtrack until a certain level.
    void     analyze          (CRef confl, vec<Lit>& out_learnt, int& out_btlevel);    // (bt = backtrack)
    void     analyzeFinal     (Lit p, vec<Lit>& out_conflict);                         // COULD THIS BE IMPLEMENTED BY THE ORDINARIY "analyze" BY SOME REASONABLE GENERALIZATION?
    bool     litRedundant     (Lit p, uint32_t abstract_levels);                       // (helper method for 'analyze()')
    lbool    search           (int nof_conflicts);                                     // Search for a given number of conflicts.
    lbool    solve_           (int limitRestart = 0);   // Main solve method (assumptions given in 'assumptions').
    void     reduceDB         ();                                                      // Reduce the set of learnt clauses.
    void     removeSatisfied  (vec<CRef>& cs);                                         // Shrink 'cs' to contain only non-satisfied clauses.
    void     rebuildOrderHeap ();

    // Maintaining Variable/Clause activity:
    //
    void     varDecayActivity ();                      // Decay all variables with the specified factor. Implemented by increasing the 'bump' value instead.
    void     varBumpActivity  (Var v, double inc);     // Increase a variable with the current 'bump' value.
    void     varBumpActivity  (Var v);                 // Increase a variable with the current 'bump' value.
    void     claDecayActivity ();                      // Decay all clauses with the specified factor. Implemented by increasing the 'bump' value instead.
    void     claBumpActivity  (Clause& c);             // Increase a clause with the current 'bump' value.

    // Operations on clauses:
    //
    void     attachClause     (CRef cr);               // Attach a clause to watcher lists.
    void     detachClause     (CRef cr, bool strict = false); // Detach a clause to watcher lists.
    void     removeClause     (CRef cr);               // Detach and free a clause.
    bool     locked           (const Clause& c) const; // Returns TRUE if a clause is a reason for some implication in the current state.
    bool     satisfied        (const Clause& c) const; // Returns TRUE if a clause is satisfied in the current state.

    void     relocAll         (ClauseAllocator& to);

    // Misc:
    //
    int      decisionLevel    ()      const; // Gives the current decisionlevel.
    uint32_t abstractLevel    (Var x) const; // Used to represent an abstraction of sets of decision levels.
    CRef     reason           (Var x) const;
    int      level            (Var x) const;
    double   progressEstimate ()      const; // DELETE THIS ?? IT'S NOT VERY USEFUL ...
    bool     withinBudget     ()      const;

    // Static helpers:
    //

    // Returns a random float 0 <= x < 1. Seed must never be 0.
    static inline double drand(double& seed) {
        seed *= 1389796;
        int q = (int)(seed / 2147483647);
        seed -= (double)q * 2147483647;
        return seed / 2147483647; }

    // Returns a random integer 0 <= x < size. Seed must never be 0.
    static inline int irand(double& seed, int size) {
        return (int)(drand(seed) * size); }

    // add by me
    int nbInitVariable;
    int nbUnitConsidered;
    vec<bool> canBeSelected;
    vec<int> intersectHit;
    vec<bool> tmpBoolVar;
    vec<int> selectorToIndex;
    vec<int> mapSelectorToIndex;

    
    int idxClauses;
    vec<litUnit> saveLitUnit;    
    vec<Lit> currAss;

    vec< vec<Lit> > recCollectedCores;
    vec<bool> isCollected;
    
    bool foundUnsat;
    int nbComputedMCS, nbCoMssRotation, nbIdentifiedByCore;
    int nbRotation;
    int nbSATCall, nbUnsatCall;
    double timeUnsat, timeSat;
    int optStrategyCoMss;    
    bool optBackBone, optRefineApprox, optAddNegation, optRefineCore, optRotation, optComputeOne;
    int removedByRefinement;
    bool solvePartialMaxSAT;
    bool lastStatus;
    bool optCoMssVerification;
    bool optUseCoreStart;

    Var controlBlockedClauses;
    Lit activeBlockedClauses;
    int limitZeroLevel;
    
    Var phantom; // an additional variable used in the refineApproximation function
    vec<Lit> unitHardPart, setOfSelector;
    vec<CRef> softClauses;

    void recursiveAnalyzeFinal(Lit l);
    
    inline void showOptions()
    {
      printf("c Active options: \n");
      printf("c Approximation refinement: %d\n", optRefineApprox);
      printf("c Negation enforcement: %d\n", optAddNegation);
      printf("c\n");
    }

    inline void showClause(Clause &c)
    {
      printf("%d: ", c.index());
      for(int j = 0 ; j<c.size() ; j++) printf("%d ", readeableLit(c[j]));        
      printf("\n");
    }

    inline void showCoreWrtLit(coreWrtLit &col){printf("%d ~~~> ", readeableLit(col.l)); showList(col.core);}
    inline void showFormula(){for(int i = 0 ; i<clauses.size() ; i++) showClause(ca[clauses[i]]);}
    inline void showBlockedClauses(){for(int i=0; i<blockedClauses.size(); i++) showClause(ca[blockedClauses[i]]);}
    inline void showLearnt(){for(int i = 0 ; i<learnts.size() ; i++) showClause(ca[learnts[i]]);}

    inline void showList(vec<Lit> &l)
    {
      for(int i = 0 ; i<l.size() ; i++) printf("%d ", readeableLit(l[i]));
      printf("\n");
    }

    inline void bigRestart()
    {
      for (int c = trail.size() - 1 ; c >= 0 ; c--)
        {
          Var x = var(trail[c]);
          assigns [x] = l_Undef;
          insertVarOrder(x); 
          canBeSelected[x] = true;
        }
      limitZeroLevel = 0;
      
      qhead = 0;
      trail.clear();
      trail_lim.clear();

      removeAllLearnt(); // clean learnt clauses
    }

    inline void restartUntil(int v)
    {
      cancelUntil(0);
      for (int c = trail.size() - 1 ; c >= v ; c--)
        {
          Var x = var(trail[c]);
          assigns [x] = l_Undef;
          insertVarOrder(x); 
        }
      
      qhead = 0;
      trail.shrink(trail.size() - v);
      trail_lim.clear();

      removeAllLearnt(); // clean learnt clauses
    }


    /**
       \return true if the problem is unsat and false otherwise       
     */
    inline void addBlockedClause(vec<Lit> &identifiedCoMssClauses)
    {            
      cancelUntil(0);      

      // we can simplified identifiedCoMssClauses
      int i, j;
      for(i = j = 0 ; i<identifiedCoMssClauses.size() ; i++)
        {
          assert(value(identifiedCoMssClauses[i]) != l_True);
          if(value(identifiedCoMssClauses[i]) == l_Undef) identifiedCoMssClauses[j++] = identifiedCoMssClauses[i];
        }
      
      identifiedCoMssClauses.shrink(i - j);
      if((optUseCoreStart || optRotation) && identifiedCoMssClauses.size() > 1)
        identifiedCoMssClauses.push(~activeBlockedClauses);
      
      if(identifiedCoMssClauses.size() > 1) createNewClause(blockedClauses, identifiedCoMssClauses, false);
      if(identifiedCoMssClauses.size() == 1) uncheckedEnqueue(identifiedCoMssClauses[0]);
      
      foundUnsat = identifiedCoMssClauses.size() == 0;
    }// addBlockedClause

    inline void removeAllLearnt()
    {
      for(int i = 0 ; i<learnts.size() ; i++)
        {
          Clause& c = ca[learnts[i]];
          detachClause(learnts[i], true);
          if(locked(c)) vardata[var(c[0])].reason = CRef_Undef;
          c.mark(1); 
          ca.free(learnts[i]);
        } 
      learnts.clear();
    }

    /**
       Add the selector on the unit literals of the formula.
       @param[out] approxCoMss, the set of aditionnal selectors
     */
    inline void manageUnitLiterals(vec<Lit> &approxCoMss)
    {
      for(int i = 0 ; i<saveLitUnit.size() ; i++)
        {
          vec<Lit> ps;
          Lit l = saveLitUnit[i].l;
          ps.push(l);
      
          // add selector
          Var v = newVar(false, SELECT_IS_DEC);
          ps.push(mkLit(v, true));
          
          approxCoMss.push(mkLit(v, false));
          
          // create new clause
          CRef cr = ca.alloc(ps, false);
          selectorToIndex.push(saveLitUnit[i].index);
          mapSelectorToIndex.push(clauses.size());
          ca[cr].selector(v);

          clauses.push(cr);
          attachClause(cr);
          ca[cr].index(saveLitUnit[i].index);
        }
    }// manageUnitLiterals


    /**
       Select a CoMss approximation with respect to the polarity
       interpretation.

       @param[out] approxCoMss, the result of the approximation
     */
    inline void selectSetOfClauses(vec<Lit> &approxCoMss)
    { 
      for(int i = 0 ; i<softClauses.size() ; i++)
        {
          clauses.push(softClauses[i]);
          attachClause(softClauses[i]);
        }
      softClauses.clear();

      // add the selector on the unsatisfiable clauses 
      for(int i = 0 ; i<clauses.size() ; i++)
        {
          Clause &c = ca[clauses[i]];
          bool isSAT = false;
          for(int j = 0 ; j<c.size() && !isSAT ; j++) 
            isSAT = var(c[j]) >= nbInitVariable || sign(c[j]) == polarity[var(c[j])];

          if(!isSAT) // adding a selector 
            { 
              // save the clause
              int idx = c.index(); 
              vec<Lit> ps;
              for(int j = 0 ; j<c.size() ; j++) ps.push(c[j]);

              // remove clause
              detachClause(clauses[i], true);
              if(locked(c)) vardata[var(c[0])].reason = CRef_Undef;
              c.mark(1); 
              ca.free(clauses[i]);

              // add selector
              Var v = newVar(false, SELECT_IS_DEC);
              ps.push(mkLit(v, true));
              approxCoMss.push(mkLit(v, false));
              
              // create new clause
              CRef cr = ca.alloc(ps, false);
              clauses[i] = cr;
              attachClause(cr);
              ca[cr].index(idx);

              mapSelectorToIndex.push(i);
              selectorToIndex.push(idx);
              ca[cr].selector(v);
            }
        }
    }// selectSetOfClauses

    /**
       Add a selector on ALL soft clauses
     */
    inline void addSelectorOnSoftClauses(vec<Lit> &approxCoMss)
    {      
      for(int i = 0 ; i<softClauses.size() ; i++) ca[softClauses[i]].mark(1);
      
      // add the selector on the unsatisfiable clauses 
      for(int i = 0 ; i<clauses.size() ; i++)
        {
          Clause &c = ca[clauses[i]];
          if(!c.mark()) continue;
          
          // save the clause
          int idx = c.index(); 
          vec<Lit> ps;
          bool wasAnUnit = false;
          for(int j = 0 ; j<c.size() && !wasAnUnit ; j++)
            {
              ps.push(c[j]);
              wasAnUnit = var(c[j]) >= nbInitVariable;
            }          
          if(wasAnUnit) continue;

          // remove clause
          detachClause(clauses[i], true);
          if(locked(c)) vardata[var(c[0])].reason = CRef_Undef;
          c.mark(1); 
          ca.free(clauses[i]);
          
          // add selector
          Var v = newVar(false, SELECT_IS_DEC);
          ps.push(mkLit(v, true));
          approxCoMss.push(mkLit(v, false));
          
          // create new clause
          CRef cr = ca.alloc(ps, false);
          clauses[i] = cr;
          attachClause(cr);
          ca[cr].index(idx);
              
          mapSelectorToIndex.push(i);
          selectorToIndex.push(idx);
          ca[cr].selector(v);
        }

      for(int i = 0 ; i<softClauses.size() ; i++) ca[softClauses[i]].mark(0);
    }// addSelector

    inline void printInfoSolver()
    {
      printf("c\nc Number of SAT call: %d\n", nbSATCall);
      printf("c Time for the SAT calls: %lf\n", timeSat);
      printf("c Number of UNSAT call: %d\n", nbUnsatCall);
      printf("c Time for the UNSAT calls: %lf\n", timeUnsat);
      printf("c Number of solver call: %d\n", (int) solves);
      printf("c Number clause removed by calling the refinement method: %d\n", removedByRefinement);
      printf("c Total time: %lf\n", cpuTime());
    }

    inline void saveOneMCSOnTheTrail()
    {
      computedMCS.push();  
      for(int i = 0 ; i<trail.size() ; i++)
	{
	  if(var(trail[i]) < nbInitVariable || var(trail[i]) == phantom) continue;          
	  if(sign(trail[i])) (computedMCS.last()).push(selectorToIndex[var(trail[i]) - nbInitVariable]);
	}
    }// saveOneMCSOnTheTrail

    inline void saveOneMCS(vec<Lit> &a)
    {
      computedMCS.push();
      for(int i = 0 ; i<a.size() ; i++) (computedMCS.last()).push(selectorToIndex[var(a[i]) - nbInitVariable]);
    }// saveOneMCS

    inline void printSolution(bool res)
    {
      printInfoSolver();
      printf("c\nc Processus finished: %lf\n", cpuTime());

      if(res){ printf("s SATISFIABLE\n"); exit(0);}

      vec<int> indexCoMss;
      indexCoMss.clear();
      
      for(int i = 0 ; i<trail.size() ; i++)
        {
          if(var(trail[i]) < nbInitVariable || var(trail[i]) == phantom) continue;          
          if(sign(trail[i])) indexCoMss.push(selectorToIndex[var(trail[i]) - nbInitVariable]);
        }
      
      printf("c Size of the coMSS: %d\n", indexCoMss.size());

      printf("s UNSATISFIABLE\n");
      printf("v ");  
      sort(indexCoMss); // print the final result (the result is ordered)    
      for(int i = 0 ; i<indexCoMss.size() ; i++) printf("%d ", indexCoMss[i]);
      printf("0\n");
    }// printSolution



    /**
       Implement the method propose in the Joao's paper (IJCAI'13)
       @param[in] l, the literal selector
     */
    inline void backBone(Var v)
    {
      Clause &c = ca[clauses[mapSelectorToIndex[v - nbInitVariable]]];
      for(int i = 0 ; i<c.size() ; i++) 
        if(value(c[i]) == l_Undef && var(c[i]) < nbInitVariable) uncheckedEnqueue(~c[i]);       
    }// backBone

    
    /**
       Implement the method propose in the Joao's paper (IJCAI'13)
       (for the extraction of all MCS).

       @param[in] l, the literal selector
     */
    inline void backBoneAllLit(Lit l, vec<Lit> &approxMSS)
    {
      Var v = var(l);
      Clause &c = ca[clauses[mapSelectorToIndex[v - nbInitVariable]]];
      for(int i = 0 ; i<c.size() ; i++) 
        if(value(c[i]) == l_Undef && var(c[i]) < nbInitVariable) approxMSS.push(~c[i]); 
    }// backBone


    /**
       Implement the method propose in the Joao's paper (IJCAI'13)
       (for the extraction of all MCS).

       @param[in] l, the literal selector
     */
    inline void backBoneAll(vec<Lit> &identifiedAsMCS, vec<Lit> &approxMSS)
    {
      for(int k = 0 ; k<identifiedAsMCS.size() ; k++) backBoneAllLit(identifiedAsMCS[k], approxMSS);
    }// backBone

    
    inline int addNegationClause(Lit l, vec<Lit> &assums)
    {
      int ret = assums.size();
      if(assums.size() <= 1) return ret;

      Clause &c = ca[clauses[mapSelectorToIndex[var(l) - nbInitVariable]]];
      for(int i = 0 ; i<c.size() ; i++)
        if(value(c[i]) == l_Undef && var(c[i]) < nbInitVariable) assums.push(~c[i]);
      
      return ret;
    }

    /**
       Put all the negation element of the first list to the second.
       @param[in] e1, the list which can be move 
       @param[out] e2, the list where is save the negation of e1
     */
    inline void putNegationList(vec<Lit> &e1, vec<Lit> &e2)
    {
      e2.clear();
      for(int i = 0 ; i<e1.size() ; i++) e2.push(~e1[i]);
    }// putNegationList

    /* Look if a list only contains selectors */
    inline bool onlyContainsSelector(vec<Lit> &e)
    {
      bool ret = true;
      for(int i = 0 ; i<e.size() && ret ; i++) ret = var(e[i]) >= nbInitVariable;
      return ret;
    }// onlyContainsSelector


    /* Look if a list only contains initial variables  */    
    inline bool onlyContainsInitVariable(vec<Lit> &e)
    {
      bool ret = false;
      for(int i = 0 ; i<e.size() && !ret ; i++) ret = var(e[i]) >= nbInitVariable;
      return !ret;
    }// onlyContainsInitVariable


    /*      
      Take a set of literal to create a new clause.
      
      @param[in] cls, the vector where is saved the clauses
      @param[in] lits, the set of literals representing the clause
      @param[in] flag, to know if it is a learnt clause
    */
    inline void createNewClause(vec<CRef> &cls, vec<Lit> &lits, bool flag = true)
    {
      CRef cr = ca.alloc(lits, flag);
      cls.push(cr);
      attachClause(cr);
    }// createNewClause

    /**
       update the list of detected coMss clauses.       
    */
    inline void collectCurrentCoMSS(vec<Lit> &identifiedCoMssClauses, int &posTrail)
    {
      for( ; posTrail<trail.size() ; posTrail++)
        {
          if(var(trail[posTrail]) < nbInitVariable || var(trail[posTrail]) == phantom) continue;          
          if(sign(trail[posTrail])) identifiedCoMssClauses.push(~trail[posTrail]);
        }
    }// collectCurrentCoMSS
       
   
    /**
       Remove the satisfied literals
     */
    inline void collectApproximation(vec<Lit> &approxCoMss, bool assumsOne = false)
    {
      int i, j;
      propagate();
      for(i = j = 0 ; i<approxCoMss.size() ; i++) 
        {
          if(value(approxCoMss[i]) != l_Undef) continue;
          if(assumsOne || model[var(approxCoMss[i])] == l_False) approxCoMss[j++] = approxCoMss[i];
          else uncheckedEnqueue(approxCoMss[i]);                
        }
      approxCoMss.shrink(i - j);      
    }

    
    /**
       Remove the satisfied literals (version to enumerate all the MCS)
    */
    inline void collectApproximationAll(vec<Lit> &approxMSS, vec<Lit> &approxCoMss, bool assumsOne = false)
    {
      int i, j;
      for(i = j = 0 ; i<approxCoMss.size() ; i++) 
        {
          if(value(approxCoMss[i]) != l_Undef) continue;
          if(assumsOne || model[var(approxCoMss[i])] == l_False) approxCoMss[j++] = approxCoMss[i];
          else approxMSS.push(approxCoMss[i]);
        }
      approxCoMss.shrink(i - j);
    }
    

    inline void transferCoMSS(vec<Lit> &mustBeTransfered, vec<Lit> &approxCoMss, vec<Lit> &identifiedCoMssClauses)
    {
      int i, j;
      bool isIn, addValue;
      vec<Lit> addToMustBeTransfered;

      for(i = j = 0 ; i<approxCoMss.size() ; i++)
        {
          isIn = addValue = value(approxCoMss[i]) == l_False;
          Var v = var(approxCoMss[i]);
          for(int k = 0 ; k<mustBeTransfered.size() && !isIn ; k++)  isIn = (v == var(mustBeTransfered[k]));
          
          if(isIn)
            {
              // printf("We need to add in identifiedCoMssClauses\n");
              identifiedCoMssClauses.push(approxCoMss[i]);
              if(addValue) addToMustBeTransfered.push(approxCoMss[i]);
            }else approxCoMss[j++] = approxCoMss[i];
        }
      approxCoMss.shrink(i - j);

      for(int i = 0 ; i<addToMustBeTransfered.size() ; i++) mustBeTransfered.push(addToMustBeTransfered[i]);
    }// transferCoMSS    

    
    void transferAndCollect(Lit l, vec<Lit> &ccore, vec<Lit> &aCoMss, vec<Lit> &idenCoMss,
                                   vec<coreWrtLit> &collectedCore);
      

    
    void makeCoreIntersection(vec<coreWrtLit> &coreList);    
    
    /**
       \return true if the approximation returned belong to the final
       coMSS
     */
    inline bool reduceApproximation(vec<Lit> &approxCoMss)
    {
      vec<Lit> saveFalse;

      int i, j;
      for(i = j = 0 ; i<approxCoMss.size() ; i++) 
        {          
          if(value(approxCoMss[i]) == l_False) saveFalse.push(approxCoMss[i]);
          if(value(approxCoMss[i]) != l_Undef) continue;
          if(model[var(approxCoMss[i])] == l_False) approxCoMss[j++] = approxCoMss[i];
        }
      approxCoMss.shrink(i - j);      

      for(int i = nbInitVariable ; i<nVars() - 1 ; i++)
        {
          if(value(i) == l_Undef && model[i] == l_True) uncheckedEnqueue(mkLit(i, false));
        }

      if(saveFalse.size()) saveFalse.copyTo(approxCoMss);         
      return saveFalse.size() != 0;
    }// reduceApproximation


    /**
       Look if we can refine the approximation given in argument. We
       suppose that the formula under the assumption is unsat!
       
       @param[in] a, coMss approximation
       \return false if the approximation is'nt minimal, true otherwise
    */
    inline bool refineApproximation(vec<Lit> &a)
    {       
      if(a.size() == 1) return true;
      canTouchLearnt = false;
      int initLearntSize = learnts.size();
      
      vec<Lit> lits;
      a.copyTo(lits);
      lits.push(mkLit(phantom, true));
      createNewClause(clauses, lits);
      ca[clauses.last()].wasHere(1);
      
      bool ret = solve(mkLit(phantom, false));
      if(ret) // reduce the approximation
        {       
          ret = !reduceApproximation(a); 
          removedByRefinement += lits.size() - 1 - a.size();
        }else removePhantomFromTrail();
     
      // remove the additional clause and the associated learnt
      assert(ca[clauses.last()].wasHere());
      removeClause(clauses.last());
      clauses.pop();
      
      while(learnts.size() > initLearntSize)
        {
          removeClause(learnts.last());
          learnts.pop();          
        }
      canTouchLearnt = true;
      return !ret;
    }// refineApproximation


    inline void removePhantomFromTrail()
    {
      for(int i = 0 ; i<trail.size() ; i++)
        if(var(trail[i]) == phantom)
          {
            trail[i] = trail.last();
            trail.pop();
            qhead--;
          }
      assigns[phantom] = l_Undef;    
    }// removePhantomFromTrail

    inline int currentNbDetectedClauses()
    {
      int cpt = 0;
      for(int i = 0 ; i<trail.size() ; i++)
        {
          if(var(trail[i]) < nbInitVariable || var(trail[i]) == phantom) continue;
          if(sign(trail[i])) cpt++;
        }
      return cpt;
    }

    inline void printIndexWrtSelectors(vec<Lit> &a)
    {
      vec<int> indexCoMss;      
      for(int i = 0 ; i<a.size() ; i++) indexCoMss.push(selectorToIndex[var(a[i]) - nbInitVariable]);
      sort(indexCoMss);

      for(int i = 0 ; i<indexCoMss.size() ; i++) printf("%d ", indexCoMss[i]);
      printf("0\n");
    }// printIndexWrtSelectors
    
    
    inline void selectTransition(vec<Lit> &assums)
    {
      int pos = 0;
      for(int i = 1 ; i<assums.size() ; i++) if(activity[var(assums[pos])] < activity[var(assums[i])]) pos = i;
      
      Lit l = assums[pos];
      assums[pos] = assums[0];
      assums[0] = l;
    }// selectTransition

    
    void coMssVerification(vec<Lit> &assums, vec<Lit> &setOfSelector);    
    void recursiveConflict(vec<Lit> &approxCoMss);
    void recursiveConflictAndNegation(vec<Lit> &approxCoMss);
    void destructiveAndNegation(vec<Lit> &approxCoMss);
    void constructive(vec<Lit> &approxCoMss);
    void workOnUnitLiteralsConstructive(vec<int> &indexCoMss);

    // function to enumerate all the coMSS
    void computeNewCoMSS(vec<Lit> &coMssFound, vec< vec<Lit> > &productFree);
    void computeNewCoMSS_acc(vec<Lit> &currentCoMss, vec< vec<Lit> > &productFree, vec< vec<Lit> > &res);
    
    bool rotationProcessus(SolverRL &rlSolver, vec<Lit> &coMssFound, vec<coreWrtLit> &ccore);
    
    inline void addCoMssAndBlockIt(vec<Lit> &coMssFound)
    {
      ++nbComputedMCS;
      printf("c MCS(%d/%d): ", nbComputedMCS, nbCoMssRotation); printIndexWrtSelectors(coMssFound);
      // coMssVerification(coMssFound, setOfSelector);
      addBlockedClause(coMssFound);
    }// addCoMssAndBlockIt

#define DDD 0
    /**
       Return false if the initialization is impossible.
     */
    inline void initializeProcedure(vec<Lit> &select, vec<Lit> &approxMSS, vec<Lit> &approxMCS,
                                    vec<Lit> &assums, vec<Lit> &id, vec<Lit> &lCore, vec<coreWrtLit> &coverCore)
    {
      id.clear();
      approxMSS.clear();
      approxMCS.clear();
      coverCore.clear();

      cancelUntil(0);
      for(int i = 0 ; i<select.size() ; i++) 
        {
          Lit l = select[i];
          if(model[var(l)] == l_False) approxMCS.push(l); else approxMSS.push(l);
        }
      if(!optUseCoreStart){approxMCS.copyTo(assums);return;}

      //printf("approxMCS: "); printIndexWrtSelectors(approxMCS);
      //printf("approxMSS: "); printIndexWrtSelectors(approxMSS);

      // useCoreStart
      vec<Lit> nowIdentified, notNowIdentified;
      for(int i = 0 ; i<approxMCS.size() ; i++) canBeSelected[var(approxMCS[i])] = false;
      for(int i = 0 ; i<approxMCS.size() ; i++)
        {
          if(recCollectedCores[var(approxMCS[i])].size())
            {
              vec<Lit> &tmp = recCollectedCores[var(approxMCS[i])];

              //printf("%d: ", selectorToIndex[var(approxMCS[i]) - nbInitVariable]);
              //printIndexWrtSelectors(tmp);
              
              bool isInMSS = true;
              for(int j = 0 ; j<tmp.size() && isInMSS ; j++) isInMSS = canBeSelected[var(tmp[j])];
              
              if(isInMSS)
                {
                  nowIdentified.push(approxMCS[i]);
                  coverCore.push();
                  (coverCore.last()).l = approxMCS[i];
                  tmp.copyTo((coverCore.last()).core);
                  isCollected[var(approxMCS[i])] = true;
                }else notNowIdentified.push(approxMCS[i]);
            }else notNowIdentified.push(approxMCS[i]);
        }
      for(int i = 0 ; i<approxMCS.size() ; i++) canBeSelected[var(approxMCS[i])] = true;
      notNowIdentified.copyTo(approxMCS);
      nowIdentified.copyTo(id);

      nbIdentifiedByCore += id.size();
      if(id.size())
        {
          if(solve(approxMSS, approxMCS)) approxMCS.clear();
          
          approxMCS.copyTo(assums);
          intersectConflict(assums, conflict);
          conflict.copyTo(lCore);
        }else approxMCS.copyTo(assums);    
    }// initializeProcedure


    inline void intersectConflict(vec<Lit> &a, vec<Lit> &c)
    {
      int i, j;
      for(i = j = 0 ; i<a.size() ; i++)
        {
          bool isIn = false;
          Var v = var(a[i]);
          for(int k = 0 ; k<c.size() && !isIn ; k++) isIn = var(c[k]) == v;
          
          if(isIn) a[j++] = a[i];
        }
      a.shrink(i - j);
    }// intersectConflict


    inline bool refineApproximationAll(vec<Lit> &approxMSS, vec<Lit> &approxMCS, vec<Lit> &currentApproxMCS)
    { 
      int i, j;
      
      if(currentApproxMCS.size() <= 1) return true;
      canTouchLearnt = false;
      int initLearntSize = learnts.size();

      vec<Lit> lits;
      currentApproxMCS.copyTo(lits);
      lits.push(mkLit(phantom, true));
      createNewClause(clauses, lits);
      ca[clauses.last()].wasHere(1);
      
      approxMSS.push(mkLit(phantom, false));
      bool ret = solve(approxMSS);
      approxMSS.pop();
      
      if(ret) // reduce the approximation
        { 
          vec<Lit> saveFalse;          
          
          for(i = j = 0 ; i<currentApproxMCS.size() ; i++) 
            {          
              if(value(currentApproxMCS[i]) == l_False) saveFalse.push(currentApproxMCS[i]);
              if(value(currentApproxMCS[i]) != l_Undef) continue;
              if(model[var(currentApproxMCS[i])] == l_False) currentApproxMCS[j++] = currentApproxMCS[i];
            }
          currentApproxMCS.shrink(i - j);
          
          for(i = j = 0 ; i<approxMCS.size() ; i++)
            if(model[var(approxMCS[i])] == l_True) approxMSS.push(approxMCS[i]); else approxMCS[j++] = approxMCS[i];
          approxMCS.shrink(i - j);
          
          if(saveFalse.size()) saveFalse.copyTo(currentApproxMCS);
          ret = (saveFalse.size() == 0);
          removedByRefinement += lits.size() - 1 - currentApproxMCS.size();
        }else removePhantomFromTrail();
      
      // remove the additional clause and the associated learnt
      assert(ca[clauses.last()].wasHere());
      removeClause(clauses.last());
      clauses.pop();
      
      while(learnts.size() > initLearntSize)
        {
          removeClause(learnts.last());
          learnts.pop();          
        }
      canTouchLearnt = true;
      return !ret;
    }// refineApproximationAll

 public:

    // Function used by Coquillas

    bool optSaveMCS;
    vec< vec<int> > computedMCS;

    bool isNotInCoMSS(int idx);

    /**
       Put the (exactly) unit literals present in saveLitUnit in units.
       
       @param[out] units, the set of unit literal collected (WARNING: units is cleared)
    */
    inline void useAsCompleteSolver()
    {
      for(int i = 0 ; i<saveLitUnit.size() ; i++)
        { 
          Lit l = saveLitUnit[i].l;
          ok = value(l) != l_False;
          
          if(!ok) return;
          if(value(l) == l_Undef) uncheckedEnqueue(l);
        }
      
      CRef cr = propagate();
      ok = cr == CRef_Undef;
    }// collectInitialUnitLit


    // end of the Coquillas part
    
    void minCoMssIteration();
    bool extractCoMSS();
    void enumAllCoMssBlocked();
    void enumAllCoMssTree();
    inline void setSoftInstance(bool soft){solvePartialMaxSAT = soft;}
    inline void initInfoSolver(){nbInitVariable = nVars();}
    inline void changeOption(int opt, bool rApprox, bool rCore, bool rot, bool coreSave)
    {optStrategyCoMss = opt; optRefineApprox = rApprox; optRefineCore = rCore;
      optRotation = rot; optUseCoreStart = coreSave;}


    inline int initCoMssExtractor(vec<Lit> &setOfSelector)
    {
      if(verbosity > 0)
	{
	  printf("c We considere a instance for: %s\nc\n", (solvePartialMaxSAT) ? "Partial MaxSAT" : "MaxSAT"); 
	  printf("c Number of literals %d\n", nVars());
	  printf("c Number of clauses %d\n", clauses.size());
	  printf("c Number of unit clauses %d\n", saveLitUnit.size());
	  printf("c\n");
	}

      nbInitVariable = nVars();
      nbRotation = 0;
      manageUnitLiterals(setOfSelector); // add the clause associated to the unit literals
      for(int i = 0 ; i<unitHardPart.size() ; i++) 
        {
          if(value(unitHardPart[i]) == l_False){printf("s HARD PART UNSATISFIABLE\n"); exit(30);}
          if(value(unitHardPart[i]) == l_Undef) uncheckedEnqueue(unitHardPart[i]);
        }
  
      bool res = solve();
      bigRestart();
      remove_satisfied = true;

      if(!res && solvePartialMaxSAT){printf("s HARD PART UNSATISFIABLE\n"); exit(30);}
      for(int i = 0 ; i<setOfSelector.size() && res ; i++) res = model[var(setOfSelector[i])] != l_False;  
      if(res && !solvePartialMaxSAT){ assert(0); printSolution(true);}

      // we adjust the polarity for the approximation
      if(res) for(int i = 0 ; i<model.size() ; i++) polarity[i] = (model[i] != l_True); 

      // the hard unit literals must be assigned to true
      for(int i = 0 ; i<unitHardPart.size() ; i++) 
        if(value(unitHardPart[i]) == l_Undef) uncheckedEnqueue(unitHardPart[i]);

      // initialization
      addSelectorOnSoftClauses(setOfSelector);
      phantom = newVar(false, SELECT_IS_DEC);
      controlBlockedClauses = newVar(false, SELECT_IS_DEC);
      activeBlockedClauses = mkLit(controlBlockedClauses, false);

      return setOfSelector.size();
    }
};


//=================================================================================================
// Implementation of inline methods:

inline CRef Solver::reason(Var x) const { return vardata[x].reason; }
inline int  Solver::level (Var x) const { return vardata[x].level; }

inline void Solver::insertVarOrder(Var x) {if (!order_heap.inHeap(x) && decision[x]) order_heap.insert(x); }

inline void Solver::varDecayActivity() { var_inc *= (1 / var_decay); }
inline void Solver::varBumpActivity(Var v) { varBumpActivity(v, var_inc); }
inline void Solver::varBumpActivity(Var v, double inc) 
{
  if ((activity[v] += inc) > 1e100)
    {
      // Rescale:
      for (int i = 0; i < nVars(); i++) activity[i] *= 1e-100;
      var_inc *= 1e-100; 
    }
  
  // Update order_heap with respect to new activity:
  if (order_heap.inHeap(v)) order_heap.decrease(v);
}

inline void Solver::claDecayActivity() { cla_inc *= (1 / clause_decay); }
inline void Solver::claBumpActivity (Clause& c) {
        if ( (c.activity() += cla_inc) > 1e20 ) {
            // Rescale:
            for (int i = 0; i < learnts.size(); i++)
                ca[learnts[i]].activity() *= 1e-20;
            cla_inc *= 1e-20; } }

inline void Solver::checkGarbage(void){ return checkGarbage(garbage_frac); }
inline void Solver::checkGarbage(double gf)
{
  if(ca.wasted() > ca.size() * gf) garbageCollect(); 
}

// NOTE: enqueue does not set the ok flag! (only public methods do)
 inline bool Solver::enqueue(Lit p, CRef from)
 { return value(p) != l_Undef ? value(p) != l_False : (uncheckedEnqueue(p, from), true); }
 inline bool Solver::addClause(const vec<Lit>& ps)    { ps.copyTo(add_tmp); return addClause_(add_tmp); }
 inline bool Solver::addEmptyClause  ()                      { add_tmp.clear(); return addClause_(add_tmp); }
 inline bool Solver::addClause(Lit p){ add_tmp.clear(); add_tmp.push(p); return addClause_(add_tmp); }
 inline bool Solver::addClause (Lit p, Lit q)
 { add_tmp.clear(); add_tmp.push(p); add_tmp.push(q); return addClause_(add_tmp); }
 inline bool Solver::addClause(Lit p, Lit q, Lit r)
 { add_tmp.clear(); add_tmp.push(p); add_tmp.push(q); add_tmp.push(r); return addClause_(add_tmp); }
 inline bool Solver::locked(const Clause& c) const
 { return value(c[0]) == l_True && reason(var(c[0])) != CRef_Undef && ca.lea(reason(var(c[0]))) == &c; }
 inline void     Solver::newDecisionLevel()                      { trail_lim.push(trail.size()); }
 
 inline int Solver::decisionLevel ()      const   { return trail_lim.size(); }
 inline uint32_t Solver::abstractLevel (Var x) const   { return 1 << (level(x) & 31); }
 inline lbool Solver::value         (Var x) const   { return assigns[x]; }
 inline lbool Solver::value         (Lit p) const   { return assigns[var(p)] ^ sign(p); }
 inline lbool Solver::modelValue    (Var x) const   { return model[x]; }
 inline lbool Solver::modelValue    (Lit p) const   { return model[var(p)] ^ sign(p); }
 inline int Solver::nAssigns      ()      const   { return trail.size(); }
 inline int Solver::nClauses      ()      const   { return clauses.size(); }
 inline int Solver::nLearnts      ()      const   { return learnts.size(); }
 inline int Solver::nVars         ()      const   { return vardata.size(); }
 inline int Solver::nFreeVars()const{return (int)dec_vars - (trail_lim.size() == 0 ? trail.size() : trail_lim[0]); }
 inline void Solver::setPolarity   (Var v, bool b) { polarity[v] = b; }
 inline void Solver::setDecisionVar(Var v, bool b) 
 { 
   if(b && !decision[v]) dec_vars++; else if (!b && decision[v]) dec_vars--;
   
   decision[v] = b;
   insertVarOrder(v);
 }
inline void     Solver::setConfBudget(int64_t x){ conflict_budget    = conflicts    + x; }
inline void     Solver::setPropBudget(int64_t x){ propagation_budget = propagations + x; }
inline void     Solver::interrupt(){ asynch_interrupt = true; }
inline void     Solver::clearInterrupt(){ asynch_interrupt = false; }
inline void     Solver::budgetOff(){ conflict_budget = propagation_budget = -1; }
inline bool     Solver::withinBudget() const {
    return !asynch_interrupt &&
           (conflict_budget    < 0 || conflicts < (uint64_t)conflict_budget) &&
           (propagation_budget < 0 || propagations < (uint64_t)propagation_budget); }

// FIXME: after the introduction of asynchronous interrruptions the solve-versions that return a
// pure bool do not give a safe interface. Either interrupts must be possible to turn off here, or
// all calls to solve must return an 'lbool'. I'm not yet sure which I prefer.
inline bool     Solver::solve         ()                    { budgetOff(); assumptions.clear(); return solve_() == l_True; }
inline bool     Solver::solve         (Lit p)               { budgetOff(); assumptions.clear(); assumptions.push(p); return solve_() == l_True; }
inline bool     Solver::solve         (Lit p, Lit q)        { budgetOff(); assumptions.clear(); assumptions.push(p); assumptions.push(q); return solve_() == l_True; }
inline bool     Solver::solve         (Lit p, Lit q, Lit r) { budgetOff(); assumptions.clear(); assumptions.push(p); assumptions.push(q); assumptions.push(r); return solve_() == l_True; }
 inline bool Solver::solve(const vec<Lit>& assumps)
 {
   budgetOff();
   assumps.copyTo(assumptions); 
   return solve_() == l_True; 
 }

 inline bool Solver::solve(const vec<Lit>& a1, const vec<Lit>& a2)
 {
   budgetOff();
   a1.copyTo(assumptions); 
   for(int i = 0 ; i<a2.size() ; i++) assumptions.push(a2[i]);
   return solve_(a1.size()) == l_True; 
 }// solve_


inline lbool    Solver::solveLimited  (const vec<Lit>& assumps){ assumps.copyTo(assumptions); return solve_(); }
inline bool     Solver::okay          ()      const   { return ok; }

inline void     Solver::toDimacs     (const char* file){ vec<Lit> as; toDimacs(file, as); }
inline void     Solver::toDimacs     (const char* file, Lit p){ vec<Lit> as; as.push(p); toDimacs(file, as); }
inline void     Solver::toDimacs     (const char* file, Lit p, Lit q){ vec<Lit> as; as.push(p); as.push(q); toDimacs(file, as); }
inline void     Solver::toDimacs     (const char* file, Lit p, Lit q, Lit r){ vec<Lit> as; as.push(p); as.push(q); as.push(r); toDimacs(file, as); }


//=================================================================================================
// Debug etc:


//=================================================================================================
}

#endif
