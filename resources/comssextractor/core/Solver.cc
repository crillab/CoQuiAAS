/***************************************************************************************[Solver.cc]
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

#include <stdio.h>
#include <math.h>

#include "utils/System.h"
#include "mtl/Sort.h"
#include "core/SolverTypes.h"
#include "core/SolverRL.h"
#include "core/Solver.h"

using namespace Minisat;

//=================================================================================================
// Options:


static const char* _cat = "CORE";

static DoubleOption  opt_var_decay         (_cat, "var-decay",   "The variable activity decay factor",            0.95,     DoubleRange(0, false, 1, false));
static DoubleOption  opt_clause_decay      (_cat, "cla-decay",   "The clause activity decay factor",              0.999,    DoubleRange(0, false, 1, false));
static DoubleOption  opt_random_var_freq   (_cat, "rnd-freq",    "The frequency with which the decision heuristic tries to choose a random variable", 0, DoubleRange(0, true, 1, true));
static DoubleOption  opt_random_seed       (_cat, "rnd-seed",    "Used by the random variable selection",         91648253, DoubleRange(0, false, HUGE_VAL, false));
static IntOption     opt_ccmin_mode        (_cat, "ccmin-mode",  "Controls conflict clause minimization (0=none, 1=basic, 2=deep)", 2, IntRange(0, 2));
static IntOption     opt_phase_saving      (_cat, "phase-saving", "Controls the level of phase saving (0=none, 1=limited, 2=full)", 2, IntRange(0, 2));
static BoolOption    opt_rnd_init_act      (_cat, "rnd-init",    "Randomize the initial activity", false);
static BoolOption    opt_luby_restart      (_cat, "luby",        "Use the Luby restart sequence", true);
static IntOption     opt_restart_first     (_cat, "rfirst",      "The base restart interval", 100, IntRange(1, INT32_MAX));
static DoubleOption  opt_restart_inc       (_cat, "rinc",        "Restart interval increase factor", 2, DoubleRange(1, false, HUGE_VAL, false));
static DoubleOption  opt_garbage_frac      (_cat, "gc-frac",     "The fraction of wasted memory allowed before a garbage collection is triggered",  0.20, DoubleRange(0, false, HUGE_VAL, false));


//=================================================================================================
// Constructor/Destructor:


Solver::Solver() :

    // Parameters (user settable):
    //
    verbosity        (0)
  , var_decay        (opt_var_decay)
  , clause_decay     (opt_clause_decay)
  , random_var_freq  (opt_random_var_freq)
  , random_seed      (opt_random_seed)
  , luby_restart     (opt_luby_restart)
  , ccmin_mode       (opt_ccmin_mode)
  , phase_saving     (opt_phase_saving)
  , rnd_pol          (false)
  , rnd_init_act     (opt_rnd_init_act)
  , garbage_frac     (opt_garbage_frac)
  , restart_first    (opt_restart_first)
  , restart_inc      (opt_restart_inc)

    // Parameters (the rest):
    //
  , learntsize_factor((double)1/(double)3), learntsize_inc(1.1)

    // Parameters (experimental):
    //
  , learntsize_adjust_start_confl (100)
  , learntsize_adjust_inc         (1.5)

    // Statistics: (formerly in 'SolverStats')
    //
  , solves(0), starts(0), decisions(0), rnd_decisions(0), propagations(0), conflicts(0)
  , dec_vars(0), clauses_literals(0), learnts_literals(0), max_literals(0), tot_literals(0)

  , ok                 (true)
  , cla_inc            (1)
  , var_inc            (1)
  , watches            (WatcherDeleted(ca))
  , qhead              (0)
  , simpDB_assigns     (-1)
  , simpDB_props       (0)
  , order_heap         (VarOrderLt(activity))
  , progress_estimate  (0)
  , remove_satisfied   (false)

    // Resource constraints:
    //
  , conflict_budget    (-1)
  , propagation_budget (-1)
  , asynch_interrupt   (false)
{
  limitZeroLevel = nbCoMssRotation = nbComputedMCS = removedByRefinement = nbSATCall = nbUnsatCall = idxClauses = 0;
  nbIdentifiedByCore = nbUnitConsidered = timeUnsat = timeSat = 0;
  optStrategyCoMss = 4;
  canTouchLearnt = optRefineCore = optAddNegation = optRefineApprox = optBackBone = true;
  foundUnsat = lastStatus = solvePartialMaxSAT = false;
  optSaveMCS = optComputeOne = optUseCoreStart = optCoMssVerification = false;
}


Solver::~Solver()
{
}


//=================================================================================================
// Minor methods:


// Creates a new SAT variable in the solver. If 'decision' is cleared, variable will not be
// used as a decision variable (NOTE! This has effects on the meaning of a SATISFIABLE result).
//
Var Solver::newVar(bool sign, bool dvar)
{
  int v = nVars();
  watches  .init(mkLit(v, false));
  watches  .init(mkLit(v, true ));
  assigns  .push(l_Undef);
  vardata  .push(mkVarData(CRef_Undef, 0));
  //activity .push(0);
  activity .push(rnd_init_act ? drand(random_seed) * 0.00001 : 0);
  seen     .push(0);
  isCollected.push(false);
  tmpBoolVar.push(false);
  polarity .push(sign);
  decision .push();
  trail.capacity(v+1);
  setDecisionVar(v, dvar);  
  canBeSelected.push(true);
  intersectHit.push(0);
  recCollectedCores.push();
  return v;
}


bool Solver::addClause_(vec<Lit>& ps, bool isHard)
{
  idxClauses++;
  assert(decisionLevel() == 0);
  if (!ok) return false;

  // Check if clause is satisfied and remove false/duplicate literals:
  sort(ps);
  Lit p; int i, j;
  for (i = j = 0, p = lit_Undef; i < ps.size(); i++)
    {      
      if (value(ps[i]) == l_True || ps[i] == ~p) return true;
      else if (value(ps[i]) != l_False && ps[i] != p) ps[j++] = p = ps[i];
    }
  ps.shrink(i - j);

  if(ps.size() == 1)
    {
      if(isHard && solvePartialMaxSAT) unitHardPart.push(ps[0]);
      else {
	saveLitUnit.push((litUnit) {ps[0], idxClauses});
    }
      return true;
    }
  
  if (ps.size() == 0) return ok = false;    
  
  CRef cr = ca.alloc(ps, false);  
  if(isHard || !solvePartialMaxSAT)
    {
      clauses.push(cr);
      attachClause(cr);
    }
   
  if(!isHard) softClauses.push(cr);
  ca[cr].index(idxClauses);      
  ca[cr].selector(0);  // you know that all selector indexes are greater than 0!
  
  return true;
}// addClause_


void Solver::attachClause(CRef cr) 
{
  const Clause& c = ca[cr];
  assert(c.size() > 1);
  watches[~c[0]].push(Watcher(cr, c[1]));
  watches[~c[1]].push(Watcher(cr, c[0]));
  if (c.learnt()) learnts_literals += c.size();
  else clauses_literals += c.size(); 
}


void Solver::detachClause(CRef cr, bool strict) 
{
  const Clause& c = ca[cr];
  assert(c.size() > 1);
  
  if (strict)
    {
      remove(watches[~c[0]], Watcher(cr, c[1]));
      remove(watches[~c[1]], Watcher(cr, c[0]));
    }else
    {
      // Lazy detaching: (NOTE! Must clean all watcher lists before garbage collecting this clause)
      watches.smudge(~c[0]);
      watches.smudge(~c[1]);
    }
  
  if (c.learnt()) learnts_literals -= c.size(); else clauses_literals -= c.size(); 
}


void Solver::removeClause(CRef cr) 
{
  Clause& c = ca[cr];
  detachClause(cr);
  // Don't leave pointers to free'd memory!
  if (locked(c)) vardata[var(c[0])].reason = CRef_Undef;
  c.mark(1); 
  ca.free(cr);
}


/**
   Return if the clause c is satisfied
 */
bool Solver::satisfied(const Clause& c) const 
{
  for (int i = 0; i < c.size(); i++) if (value(c[i]) == l_True) return true;
  return false; 
}// satisfied


/**
   Revert to the state at given level (keeping all assignment at
   'level' but not beyond).
*/
void Solver::cancelUntil(int level) 
{
  if (decisionLevel() > level)
    {
      for (int c = trail.size()-1; c >= trail_lim[level]; c--)
        {
          Var x  = var(trail[c]);
          isCollected[x] = false;

          assigns [x] = l_Undef;
          if (phase_saving > 1 || (phase_saving == 1) && c > trail_lim.last()) polarity[x] = sign(trail[c]);
          insertVarOrder(x); 
        }
      qhead = trail_lim[level];
      trail.shrink(trail.size() - trail_lim[level]);
      trail_lim.shrink(trail_lim.size() - level);
    } 

  if(level == 0) while(limitZeroLevel < trail.size()) canBeSelected[var(trail[limitZeroLevel++])] = false;
}// cancelUntil


//=================================================================================================
// Major methods:


Lit Solver::pickBranchLit()
{
  Var next = var_Undef;

  // Random decision: not used here
  if (drand(random_seed) < random_var_freq && !order_heap.empty())
    {
      next = order_heap[irand(random_seed,order_heap.size())];
      if (value(next) == l_Undef && decision[next]) rnd_decisions++; 
    }
  
  // Activity based decision:
  while (next == var_Undef || value(next) != l_Undef || !decision[next])
    if (order_heap.empty())
      {
        next = var_Undef;
        break;
      }else next = order_heap.removeMin();      

  return next == var_Undef ? lit_Undef : mkLit(next, rnd_pol ? drand(random_seed) < 0.5 : polarity[next]);
}// pickBranchLit


/**

  analyze : (confl : Clause*) (out_learnt : vec<Lit>&) (out_btlevel : int&)  ->  [void]
  
  Description:
    Analyze conflict and produce a reason clause.
  
    Pre-conditions:
      * 'out_learnt' is assumed to be cleared.
      * Current decision level must be greater than root level.
  
    Post-conditions:
      * 'out_learnt[0]' is the asserting literal at level 'out_btlevel'.
      * If out_learnt.size() > 1 then 'out_learnt[1]' has the greatest decision level of the 
        rest of literals. There may be others from the same level though.
  
*/
void Solver::analyze(CRef confl, vec<Lit>& out_learnt, int& out_btlevel)
{
  int pathC = 0;
  Lit p     = lit_Undef;

  // Generate conflict clause:
  //
  out_learnt.push();      // (leave room for the asserting literal)
  int index   = trail.size() - 1;

  do{
    assert(confl != CRef_Undef); // (otherwise should be UIP)
    Clause& c = ca[confl];

    if (c.learnt()) claBumpActivity(c);       
    for (int j = (p == lit_Undef) ? 0 : 1; j < c.size(); j++)
      {
        Lit q = c[j];            
        if (!seen[var(q)] && level(var(q)) > 0)
          {
            varBumpActivity(var(q));
            seen[var(q)] = 1;
            if (level(var(q)) >= decisionLevel()) pathC++;
            else out_learnt.push(q);
          }
      }
        
    // Select next clause to look at:
    while (!seen[var(trail[index--])]);
    p = trail[index+1];
    confl = reason(var(p));
    seen[var(p)] = 0;
    pathC--;
  }while (pathC > 0);
  out_learnt[0] = ~p;

  // Simplify conflict clause:
  //
  int i, j;
  out_learnt.copyTo(analyze_toclear);
  if (ccmin_mode == 2){
    uint32_t abstract_level = 0;
    for (i = 1; i < out_learnt.size(); i++)
      abstract_level |= abstractLevel(var(out_learnt[i]));//(maintain an abstraction of levels involved in conflict)

    for (i = j = 1; i < out_learnt.size(); i++)
      if (reason(var(out_learnt[i])) == CRef_Undef || !litRedundant(out_learnt[i], abstract_level))
        out_learnt[j++] = out_learnt[i];
    
  }else if (ccmin_mode == 1){
    for (i = j = 1; i < out_learnt.size(); i++){
      Var x = var(out_learnt[i]);

      if (reason(x) == CRef_Undef)
        out_learnt[j++] = out_learnt[i];
      else{
        Clause& c = ca[reason(var(out_learnt[i]))];
        for (int k = 1; k < c.size(); k++)
          if (!seen[var(c[k])] && level(var(c[k])) > 0){
            out_learnt[j++] = out_learnt[i];
            break; }
      }
    }
  }else
    i = j = out_learnt.size();

  max_literals += out_learnt.size();
  out_learnt.shrink(i - j);
  tot_literals += out_learnt.size();

  // Find correct backtrack level:
  //
  if (out_learnt.size() == 1)
    out_btlevel = 0;
  else{
    int max_i = 1;
    // Find the first literal assigned at the next-highest level:
    for (int i = 2; i < out_learnt.size(); i++)
      if (level(var(out_learnt[i])) > level(var(out_learnt[max_i])))
        max_i = i;
    // Swap-in this literal at index 1:
    Lit p             = out_learnt[max_i];
    out_learnt[max_i] = out_learnt[1];
    out_learnt[1]     = p;
    out_btlevel       = level(var(p));
  }

  for (int j = 0; j < analyze_toclear.size(); j++) seen[var(analyze_toclear[j])] = 0; // ('seen[]' is now cleared)
}


// Check if 'p' can be removed. 'abstract_levels' is used to abort early if the algorithm is
// visiting literals at levels that cannot be removed later.
bool Solver::litRedundant(Lit p, uint32_t abstract_levels)
{
  analyze_stack.clear(); analyze_stack.push(p);
  int top = analyze_toclear.size();
  while (analyze_stack.size() > 0){
    assert(reason(var(analyze_stack.last())) != CRef_Undef);
    Clause& c = ca[reason(var(analyze_stack.last()))]; analyze_stack.pop();

    for (int i = 1; i < c.size(); i++)
      {
        Lit p  = c[i];
        if (!seen[var(p)] && level(var(p)) > 0)
          {
            if (reason(var(p)) != CRef_Undef && (abstractLevel(var(p)) & abstract_levels) != 0)
              {
                seen[var(p)] = 1;
                analyze_stack.push(p);
                analyze_toclear.push(p);
              }else
              {
                for (int j = top; j < analyze_toclear.size(); j++) seen[var(analyze_toclear[j])] = 0;
                analyze_toclear.shrink(analyze_toclear.size() - top);
                return false;
              }
          }
      }
  }  
  return true;
}// litRedundant



/**
   Perform the analyze final on a literal in a recursive way.
       
   @param[in] l, the starting point
   @param[out] v, the set of visited literals
*/
void Solver::recursiveAnalyzeFinal(Lit l)
{
  Var x = var(l);      
  if(isCollected[x]) return;    

  recCollectedCores[x].zero();
  isCollected[x] = true;
  
  if(level(x) == 0) return;
  if(reason(x) == CRef_Undef) recCollectedCores[x].push(l);
  else
    {
      // collect the information
      Clause& c = ca[reason(x)];          
      for(int i = 1 ; i<c.size() ; i++) recursiveAnalyzeFinal(c[i]);

      vec<Lit> &tmp = recCollectedCores[var(l)];

      // merge it          
      for(int i = 1 ; i<c.size() ; i++)
        {
          assert(isCollected[var(c[i])]);
          vec<Lit> &tmpC = recCollectedCores[var(c[i])];
          for(int j = 0 ; j<tmpC.size() ; j++)
            if(!seen[var(tmpC[j])])
              {
                seen[var(tmpC[j])] = 1;
                tmp.push(tmpC[j]);
              }
        }
      for(int i = 0 ; i<tmp.size() ; i++) seen[var(tmp[i])] = 0;
    }
}// recursiveAnalyzeFinal


/**

  analyzeFinal : (p : Lit)  ->  [void]
  
  Description:
    Specialized analysis procedure to express the final conflict in terms of assumptions.
    Calculates the (possibly empty) set of assumptions that led to the assignment of 'p', and
    stores the result in 'out_conflict'.
*/
void Solver::analyzeFinal(Lit p, vec<Lit>& out_conflict)
{
  if(decisionLevel() == 0) return;

  // TODO
  if(!optCoMssVerification)
    {
      recursiveAnalyzeFinal(p);
      recCollectedCores[var(p)].copyTo(out_conflict);  
      out_conflict.push(p);
    }else
    {
      out_conflict.clear();
      out_conflict.push(p);
      seen[var(p)] = 1;
      for (int i = trail.size()-1; i >= trail_lim[0]; i--)
        {
          Var x = var(trail[i]);
          if (seen[x])
            {
              if (reason(x) == CRef_Undef)
                {
                  assert(level(x) > 0);
                  out_conflict.push(~trail[i]);
                }else
                {
                  Clause& c = ca[reason(x)];
                  for (int j = 1; j < c.size(); j++) if(level(var(c[j])) > 0) seen[var(c[j])] = 1;
                }
              seen[x] = 0;
            }
        }
      seen[var(p)] = 0;
    }
}// analyzeFinal


void Solver::uncheckedEnqueue(Lit p, CRef from)
{  
  assigns[var(p)] = lbool(!sign(p));
  vardata[var(p)] = mkVarData(from, decisionLevel());
  trail.push_(p);
}


/**
   propagate : [void]  ->  [Clause*]
  
   Description:
     Propagates all enqueued facts. If a conflict arises, the conflicting clause is returned,
     otherwise CRef_Undef.
  
   Post-conditions:
     - the propagation queue is empty, even if there was a conflict.
*/
CRef Solver::propagate()
{
  CRef    confl     = CRef_Undef;
  int     num_props = 0;
  watches.cleanAll();
  
  while (qhead < trail.size())
    {
      Lit            p   = trail[qhead++];     // 'p' is enqueued fact to propagate.
      vec<Watcher>&  ws  = watches[p];
      Watcher        *i, *j, *end;
      num_props++;
      
      for (i = j = (Watcher*)ws, end = i + ws.size();  i != end;)
        {
          // Try to avoid inspecting the clause:
          Lit blocker = i->blocker;
          if (value(blocker) == l_True){*j++ = *i++; continue; }

          // Make sure the false literal is data[1]:
          CRef     cr        = i->cref;
          Clause&  c         = ca[cr];

          Lit      false_lit = ~p;
          if (c[0] == false_lit) c[0] = c[1], c[1] = false_lit;
          assert(c[1] == false_lit);
          i++;

          // If 0th watch is true, then clause is already satisfied.
          Lit     first = c[0];
          Watcher w     = Watcher(cr, first);
          if (first != blocker && value(first) == l_True){*j++ = w; continue; }

          // Look for new watch:
          for (int k = 2; k < c.size(); k++)
            {
              if (value(c[k]) != l_False)
                {
                  c[1] = c[k]; c[k] = false_lit;
                  watches[~c[1]].push(w);
                  goto NextClause; 
                }
            }

          
          // Did not find watch -- clause is unit under assignment:
          *j++ = w;
          if (value(first) == l_False)
            {
              confl = cr;
              qhead = trail.size();
              
              // Copy the remaining watches:
              while (i < end) *j++ = *i++;
            }else uncheckedEnqueue(first, cr);

        NextClause:;
        }
      ws.shrink(i - j);
    }
  propagations += num_props;
  simpDB_props -= num_props;
  
  return confl;
}// propagate


/**

  reduceDB : ()  ->  [void]
  
  Description:
    Remove half of the learnt clauses, minus the clauses locked by the current assignment. Locked
    clauses are clauses that are reason to some assignment. Binary clauses are never removed.
*/
struct reduceDB_lt 
{ 
  ClauseAllocator& ca;
  reduceDB_lt(ClauseAllocator& ca_) : ca(ca_) {}
  bool operator () (CRef x, CRef y) { return ca[x].size() > 2 && (ca[y].size() == 2 || ca[x].activity() < ca[y].activity()); }
};
void Solver::reduceDB()
{
  int     i, j;
  double  extra_lim = cla_inc / learnts.size();    // Remove any clause below this activity

  sort(learnts, reduceDB_lt(ca));
  // Don't delete binary or locked clauses. From the rest, delete clauses from the first half
  // and clauses with activity smaller than 'extra_lim':
  for (i = j = 0; i < learnts.size(); i++)
    {
      Clause& c = ca[learnts[i]];
      if (c.size() > 2 && !locked(c) && (i < learnts.size() / 2 || c.activity() < extra_lim)) removeClause(learnts[i]);
      else learnts[j++] = learnts[i];
    }
  learnts.shrink(i - j);
  checkGarbage();
}// reduceDB


void Solver::removeSatisfied(vec<CRef>& cs)
{
  int i, j;
  for (i = j = 0; i < cs.size(); i++)
    {
      Clause& c = ca[cs[i]];
      if (satisfied(c)) removeClause(cs[i]); else cs[j++] = cs[i];
    }
  cs.shrink(i - j);
}// removeSatisfied


void Solver::rebuildOrderHeap()
{
  vec<Var> vs;
  for (Var v = 0; v < nVars(); v++) if(decision[v] && value(v) == l_Undef) vs.push(v);
  order_heap.build(vs);
}// rebuildOrderHeap


/**

  simplify : [void]  ->  [bool]
  
  Description:
    Simplify the clause database according to the current top-level assigment. Currently, the only
    thing done here is the removal of satisfied clauses, but more things can be put here.
*/
bool Solver::simplify()
{    
  assert(decisionLevel() == 0);

  if(!ok || propagate() != CRef_Undef) return ok = false;
  if(nAssigns() == simpDB_assigns || (simpDB_props > 0)) return true;
  
  removeSatisfied(learnts);                             // Remove satisfied clauses:
  removeSatisfied(blockedClauses);                      // Remove satisfied clauses:
  if(remove_satisfied && trail.size() - nbUnitConsidered > 10)
    {
      nbUnitConsidered = trail.size();

      int i, j;
      for (i = j = 0 ; i < clauses.size(); i++)
        {
          Clause& c = ca[clauses[i]];
          if(c.selector()) 
            {
              mapSelectorToIndex[c.selector() - nbInitVariable] = j;
              clauses[j++] = clauses[i];
            } else if(satisfied(c)) removeClause(clauses[i]); else clauses[j++] = clauses[i];
        }
      clauses.shrink(i - j); 
    }
  
  checkGarbage();
  rebuildOrderHeap();

  simpDB_assigns = nAssigns();
  simpDB_props   = clauses_literals + learnts_literals; //(shouldn't depend on stats really,but it will do for now)
  return true;
}// simplify


/**
    search : (nof_conflicts : int) (params : const SearchParams&)  ->  [lbool]
    
    Description:
      Search for a model the specified number of conflicts. 
      NOTE! Use negative value for 'nof_conflicts' indicate infinity.
    
    Output:
      'l_True' if a partial assigment that is consistent with respect to the clauseset is found. If
      all variables are decision variables, this means that the clause set is satisfiable. 'l_False'
      if the clause set is unsatisfiable. 'l_Undef' if the bound on number of conflicts is reached.
*/
lbool Solver::search(int nof_conflicts)
{
  assert(ok);
  int         backtrack_level;
  int         conflictC = 0;
  vec<Lit>    learnt_clause;
  starts++;

  for (;;)
    {
      CRef confl = propagate();
      if (confl != CRef_Undef)
        {
          // CONFLICT
          conflicts++; conflictC++;
          if (!decisionLevel()) return l_False;

          learnt_clause.clear();
          analyze(confl, learnt_clause, backtrack_level);
          cancelUntil(backtrack_level);          

          if (learnt_clause.size() == 1) uncheckedEnqueue(learnt_clause[0]);
          else
            {
              CRef cr = ca.alloc(learnt_clause, true);
              learnts.push(cr);
              attachClause(cr);
              claBumpActivity(ca[cr]);
              uncheckedEnqueue(learnt_clause[0], cr);
            }

          varDecayActivity();
          claDecayActivity();

          if (--learntsize_adjust_cnt == 0)
            {
              learntsize_adjust_confl *= learntsize_adjust_inc;
              learntsize_adjust_cnt    = (int)learntsize_adjust_confl;
              max_learnts             *= learntsize_inc;
            }
        }else
        {
          // NO CONFLICT
          if (nof_conflicts >= 0 && conflictC >= nof_conflicts || !withinBudget())
            {
              progress_estimate = progressEstimate();
              cancelUntil(assumptions.size());
              return l_Undef; 
            }

          // Simplify the set of problem clauses:
          if (!decisionLevel() && canTouchLearnt && !simplify()) return l_False;            
          if (learnts.size() - nAssigns() >= max_learnts && canTouchLearnt) reduceDB(); 
              
          Lit next = lit_Undef;
          while (decisionLevel() < assumptions.size())
            {              
              // Perform user provided assumption:
              Lit p = assumptions[decisionLevel()];
              if (value(p) == l_True) newDecisionLevel(); // Dummy decision level:
              else if (value(p) == l_False)
                {
                  analyzeFinal(~p, conflict); 
                  if(!conflict.size()) conflict.push(~p);
                  return l_False;
                }
              else {next = p; break;}
            }

          if (next == lit_Undef)
            {
              // New variable decision:
              decisions++;
              next = pickBranchLit();
                
              if(next == lit_Undef) return l_True; // Model found:
              if(var(next) >= nbInitVariable && sign(next)) next = ~next; 
            }

          // Increase decision level and enqueue 'next'
          newDecisionLevel();
          uncheckedEnqueue(next);
        }
    }
}


double Solver::progressEstimate() const
{
    double  progress = 0;
    double  F = 1.0 / nVars();

    for (int i = 0; i <= decisionLevel(); i++){
        int beg = i == 0 ? 0 : trail_lim[i - 1];
        int end = i == decisionLevel() ? trail.size() : trail_lim[i];
        progress += pow(F, i) * (end - beg);
    }

    return progress / nVars();
}

/*
  Finite subsequences of the Luby-sequence:

  0: 1
  1: 1 1 2
  2: 1 1 2 1 1 2 4
  3: 1 1 2 1 1 2 4 1 1 2 1 1 2 4 8
  ...


 */

static double luby(double y, int x){

    // Find the finite subsequence that contains index 'x', and the
    // size of that subsequence:
    int size, seq;
    for (size = 1, seq = 0; size < x+1; seq++, size = 2*size+1);

    while (size-1 != x){
        size = (size-1)>>1;
        seq--;
        x = x % size;
    }

    return pow(y, seq);
}

// NOTE: assumptions passed in member-variable 'assumptions'.
lbool Solver::solve_(int limitRestart)
{
  model.clear();
  conflict.clear();
  if (!ok || foundUnsat) return l_False;

  if(decisionLevel() > limitRestart) cancelUntil(limitRestart);
  double timeStart = cpuTime();

  //if(!solves)
  max_learnts = nClauses() * learntsize_factor;
  learntsize_adjust_confl   = learntsize_adjust_start_confl;
  learntsize_adjust_cnt     = (int)learntsize_adjust_confl;
  lbool   status            = l_Undef;

  solves++;

  // Search:
  int curr_restarts = 0;
  while (status == l_Undef)
    {
      double rest_base = luby_restart ? luby(restart_inc, curr_restarts) : pow(restart_inc, curr_restarts);
      status = search(rest_base * restart_first);
      if (!withinBudget()) break;
      curr_restarts++;
    }
  
  if (status == l_True)
    {
      nbSATCall++;
      timeSat += cpuTime() - timeStart;

      // Extend & copy model:
      model.growTo(nVars());
      for (int i = 0; i < nVars(); i++) if(value(i) != l_Undef) model[i] = value(i); else model[i] = l_True;        
      
      currAss.clear();
      for(int i = 0 ; i<nVars() ; i++) currAss.push(mkLit(i, model[i] == l_False));      
    }else
    {
      timeUnsat += cpuTime() - timeStart;
      nbUnsatCall++;
    }
  
  lastStatus = status == l_True;
  cancelUntil(limitRestart);
  return status;
}// solve_

//=================================================================================================
// Writing CNF to DIMACS:
// 
// FIXME: this needs to be rewritten completely.

static Var mapVar(Var x, vec<Var>& map, Var& max)
{
  if (map.size() <= x || map[x] == -1)
    {
      map.growTo(x+1, -1);
      map[x] = max++;
    }
  return map[x];
}// mapVar


void Solver::toDimacs(FILE* f, Clause& c, vec<Var>& map, Var& max)
{
  if (satisfied(c)) return;

  for (int i = 0; i < c.size(); i++)
    if (value(c[i]) != l_False) fprintf(f, "%s%d ", sign(c[i]) ? "-" : "", mapVar(var(c[i]), map, max)+1);
  fprintf(f, "0\n");
}// toDimacs


void Solver::toDimacs(const char *file, const vec<Lit>& assumps)
{
  FILE* f = fopen(file, "wr");
  if (f == NULL) fprintf(stderr, "could not open file %s\n", file), exit(1);
  toDimacs(f, assumps);
  fclose(f);
}// toDimacs


void Solver::toDimacs(FILE* f, const vec<Lit>& assumps)
{
  // Handle case when solver is in contradictory state:
  if (!ok)
    {
      fprintf(f, "p cnf 1 2\n1 0\n-1 0\n");
      return; 
    }

  vec<Var> map; Var max = 0;

  // Cannot use removeClauses here because it is not safe
  // to deallocate them at this point. Could be improved.
  int cnt = 0;
  for (int i = 0; i < clauses.size(); i++) if (!satisfied(ca[clauses[i]])) cnt++;
        
  for (int i = 0; i < clauses.size(); i++)
    if (!satisfied(ca[clauses[i]]))
      {
        Clause& c = ca[clauses[i]];
        for (int j = 0; j < c.size(); j++)
          if (value(c[j]) != l_False) mapVar(var(c[j]), map, max);
      }

  // Assumptions are added as unit clauses:
  cnt += assumptions.size();
  fprintf(f, "p cnf %d %d\n", max, cnt);


  for (int i = 0; i < assumptions.size(); i++)
    {
      assert(value(assumptions[i]) != l_False);
      fprintf(f, "%s%d 0\n", sign(assumptions[i]) ? "-" : "", mapVar(var(assumptions[i]), map, max)+1);
    }

  for (int i = 0; i < clauses.size(); i++) toDimacs(f, ca[clauses[i]], map, max);
  if (verbosity > 0) printf("Wrote %d clauses with %d variables.\n", cnt, max);
}


//=================================================================================================
// Garbage Collection methods:

void Solver::relocAll(ClauseAllocator& to)
{
  // All watchers:
  //
  // for (int i = 0; i < watches.size(); i++)
  watches.cleanAll();
  for (int v = 0; v < nVars(); v++)
    for (int s = 0; s < 2; s++)
      {
        Lit p = mkLit(v, s);
        // printf(" >>> RELOCING: %s%d\n", sign(p)?"-":"", var(p)+1);
        vec<Watcher>& ws = watches[p];
        for (int j = 0; j < ws.size(); j++) ca.reloc(ws[j].cref, to);
      }

  // All reasons:
  //
  for (int i = 0; i < trail.size(); i++)
    {
      Var v = var(trail[i]);
      
      if (reason(v) != CRef_Undef && (ca[reason(v)].reloced() || locked(ca[reason(v)])))
        ca.reloc(vardata[v].reason, to);
    }


  for (int i = 0; i < learnts.size(); i++) ca.reloc(learnts[i], to); // All learnt    
  for (int i = 0; i < clauses.size(); i++) ca.reloc(clauses[i], to); // All original
  for (int i = 0; i < blockedClauses.size(); i++) ca.reloc(blockedClauses[i], to); // All the blocked clauses
  for (int i = 0; i < softClauses.size(); i++) ca.reloc(softClauses[i], to); // All soft
}// relocAll


void Solver::garbageCollect()
{
  // Initialize the next region to a size corresponding to the estimated utilization degree. This
  // is not precise but should avoid some unnecessary reallocations for the new region:
  ClauseAllocator to(ca.size() - ca.wasted()); 

  relocAll(to);
  if (verbosity >= 2)
    printf("|  Garbage collection:   %12d bytes => %12d bytes             |\n", 
           ca.size()*ClauseAllocator::Unit_Size, to.size()*ClauseAllocator::Unit_Size);
  to.moveTo(ca);
}// garbageCollect


/**
   Extract a coMSS of the current formula.
 */
void Solver::extractCoMSS()
{
  vec<Lit> approxCoMss;

  optComputeOne = true;
  nbInitVariable = nVars();
  
  if (verbosity > 0)
    {
      printf("c CoMSS extraction start ...\nc\n"); 
      printf("c We considere a instance for: %s\nc\n", (solvePartialMaxSAT) ? "Partial MaxSAT" : "MaxSAT"); 
      printf("c Number of literals %d\n", nVars());
      printf("c Number of clauses %d\n", clauses.size());
      printf("c Number of unit clauses %d\n", saveLitUnit.size());
      printf("c\n");
    }

  manageUnitLiterals(approxCoMss); // add the clause associated to the unit literals  
  for(int i = 0 ; i<unitHardPart.size() ; i++) 
    {
      if(value(unitHardPart[i]) == l_False){printf("s HARD PART UNSATISFIABLE\n"); exit(30);}
      if(value(unitHardPart[i]) == l_Undef) uncheckedEnqueue(unitHardPart[i]);
    }
  
  bool res = solve();
  bigRestart();

  if(!res && solvePartialMaxSAT){printf("s HARD PART UNSATISFIABLE\n"); exit(30);}
  for(int i = 0 ; i<approxCoMss.size() && res ; i++) res = model[var(approxCoMss[i])] != l_False;  
  if(res && !solvePartialMaxSAT){printSolution(true);}
  if(res) for(int i = 0 ; i<model.size() ; i++) polarity[i] = (model[i] != l_True); 
  
  // compute a coMSS approximation  
  selectSetOfClauses(approxCoMss);
  for(int i = 0 ; i<unitHardPart.size() ; i++)
    if(value(unitHardPart[i]) == l_Undef) uncheckedEnqueue(unitHardPart[i]);
  
  if (verbosity > 0)
    {
      printf("c Size of the approximation: %d\n", approxCoMss.size());
      printf("c Time to obtained the approximation: %lf\n", cpuTime());
      printf("c\n");
    }
  
  phantom = newVar();
  if (verbosity > 0) showOptions();

  // extract a coMSS
  if(optStrategyCoMss == 1) destructiveAndNegation(approxCoMss);
  if(optStrategyCoMss == 2) recursiveConflict(approxCoMss);
  if(optStrategyCoMss == 3) constructive(approxCoMss);   
  if(optStrategyCoMss == 4) recursiveConflictAndNegation(approxCoMss);

  if (verbosity > 0) printSolution(false);
  if (optSaveMCS) saveOneMCSOnTheTrail();
}// extractCoMSS


/**
   Iterate the processus of coMSS extraction in order to extract a
   little coMSS.
*/
void Solver::minCoMssIteration()
{
  printf("c All CoMSS extraction start ...\nc\n"); 
  printf("c We considere a instance for: %s\nc\n", (solvePartialMaxSAT) ? "Partial MaxSAT" : "MaxSAT"); 
  printf("c Number of literals %d\n", nVars());
  printf("c Number of clauses %d\n", clauses.size());
  printf("c Number of unit clauses %d\n", saveLitUnit.size());
  printf("c\n");

  manageUnitLiterals(setOfSelector); // add the clause associated to the unit literals
  for(int i = 0 ; i<unitHardPart.size() ; i++) 
    {
      if(value(unitHardPart[i]) == l_False){printf("s HARD PART UNSATISFIABLE\n"); exit(30);}
      if(value(unitHardPart[i]) == l_Undef) uncheckedEnqueue(unitHardPart[i]);
    }
  
  bool res = solve();
  bigRestart();

  if(!res && solvePartialMaxSAT){printf("s HARD PART UNSATISFIABLE\n"); exit(30);}
  for(int i = 0 ; i<setOfSelector.size() && res ; i++) res = model[var(setOfSelector[i])] != l_False;  
  if(res && !solvePartialMaxSAT){ assert(0); printSolution(true);}
  if(res) for(int i = 0 ; i<model.size() ; i++) polarity[i] = (model[i] != l_True); // we adjust the polarity for the approximation

  // the hard unit literals must be assigned to true
  for(int i = 0 ; i<unitHardPart.size() ; i++) if(value(unitHardPart[i]) == l_Undef) uncheckedEnqueue(unitHardPart[i]);

  // initialization
  int nbRun = 0;  
  addSelectorOnSoftClauses(setOfSelector);
  phantom = newVar(false, false);
  
  // extract a coMSS
  int initSzAssums = setOfSelector.size(), bestSizeCoMss = setOfSelector.size();
  vec<Lit> assums, approxCoMss, saveUnitSelectors;
  vec<Lit> identifiedCoMssClauses, bestCoMss; // best w.r.t. size  
  int posTrail = trail.size(), limitRestart = 0;
  bool isCoMss = true;  
  
  while(solve())
    {
      restartUntil(limitRestart);
      
      // prepare the next stage
      identifiedCoMssClauses.clear();
      limitRestart = trail.size();
           
      for(int i = 0 ; i<setOfSelector.size() ; i++)
        if(model[var(setOfSelector[i])] == l_Undef) approxCoMss.push(setOfSelector[i]);
      
      setOfSelector.copyTo(approxCoMss);
      collectApproximation(approxCoMss); // WARNING: we need to remove the satisfied selector for the core technique
             
      approxCoMss.copyTo(assums); 
      posTrail = 0;
      for( ; posTrail<trail.size() ; posTrail++)
        {
          if(var(trail[posTrail]) < nbInitVariable || var(trail[posTrail]) == phantom) continue;          
          if(sign(trail[posTrail])) identifiedCoMssClauses.push(~trail[posTrail]);
        }
      
      while(assums.size())
        {
          for(int i = 0 ; i<assums.size() ; i++) assert(!sign(assums[i])); // DEBUG
          bool assumsIsCoMSS = (assums.size() < 1);
          
          if(optRefineApprox) assumsIsCoMSS = refineApproximation(assums);          
      
          if(!assumsIsCoMSS)
            {
              selectTransition(assums);
              assums[0] = ~assums[0];          
              initSzAssums = (optAddNegation) ? assums.size() : addNegationClause(assums[0], assums);
            }
                
          if(!assumsIsCoMSS && !solve(assums))
            {         
              if(onlyContainsSelector(conflict)) putNegationList(conflict, assums);                
              else
                {      
                  if(onlyContainsInitVariable(conflict)) uncheckedEnqueue(~assums[0]); 
                  else
                    {                  
                      conflict.push(~assums[0]);
                      createNewClause(blockedClauses, conflict);
                    }
              
                  assums.shrink(assums.size() - initSzAssums);
                  assums[0] = assums.last();
                  assums.pop();
                }

              for(int i = 0 ; i<assums.size() ; i++) assert(!sign(assums[i]));
            } else
            { 
              if(assumsIsCoMSS)
                {
                  for(int i = 0 ; i<assums.size() ; i++)
                    {                  
                      if(value(assums[i]) == l_Undef) uncheckedEnqueue(~assums[i]);
                      if(optBackBone) backBone(var(assums[i])); // backbone method proposed by joao
                    }
                }else
                {                  
                  if(value(assums[0]) == l_Undef) uncheckedEnqueue(assums[0]); // transition clause          
                  if(optBackBone) backBone(var(assums[0])); // backbone method proposed by joao
                }
          
              collectApproximation(approxCoMss, assumsIsCoMSS); // collect the approximation
              
              // compute a new core with the current coMss approximation
              solve(approxCoMss);          
              putNegationList(conflict, assums);
            }
      
          // update the list of detected coMss clauses
          for( ; posTrail<trail.size() ; posTrail++)
            {
              if(var(trail[posTrail]) < nbInitVariable || var(trail[posTrail]) == phantom) continue;          
              if(sign(trail[posTrail])) identifiedCoMssClauses.push(~trail[posTrail]);
            }
        }  

      ++nbRun;  
      if(isCoMss)
        {
          coMssVerification(identifiedCoMssClauses, setOfSelector);          
          printf("c MCS(%d): ", nbRun); printIndexWrtSelectors(identifiedCoMssClauses);

          if(bestSizeCoMss > identifiedCoMssClauses.size())
            {
              bestSizeCoMss = identifiedCoMssClauses.size();
              identifiedCoMssClauses.copyTo(bestCoMss);
            }
        }//else {printf("t "); printIndexWrtSelectors(identifiedCoMssClauses);}
  
      restartUntil(limitRestart);
      
      // we can simplified identifiedCoMssClauses
      int i, j;
      for(i = j = 0 ; i<identifiedCoMssClauses.size() ; i++)
        {
          assert(value(identifiedCoMssClauses[i]) != l_True);
          if(value(identifiedCoMssClauses[i]) == l_Undef) identifiedCoMssClauses[j++] = identifiedCoMssClauses[i];
        }
      identifiedCoMssClauses.shrink_(i - j);
      assert(identifiedCoMssClauses.size());

      if(identifiedCoMssClauses.size() > 1)
        {
          CRef cr = ca.alloc(identifiedCoMssClauses, false);
          clauses.push(cr);
          attachClause(cr);      
        }else saveUnitSelectors.push(identifiedCoMssClauses[0]);    
  
      // the unit literals
      for(int i = 0 ; i<saveUnitSelectors.size() ; i++)
        if(value(saveUnitSelectors[i]) == l_Undef) uncheckedEnqueue(saveUnitSelectors[i]);
    }
  
  printf("c Number of visited coMSS: %d\n", nbRun);
  printf("o %d\n", bestSizeCoMss);
  printf("s OPTIMUM FOUND\n");
  printf("v "); printIndexWrtSelectors(bestCoMss);
}// minCoMssIteration



/**
   Look in all MSS if idx is present
 */
bool Solver::isNotInCoMSS(int idx)
{
  int nbRun = 0;
  
  // variable initialization
  if(verbosity > 0){printf("c All CoMSS extraction start ...\nc\n"); showOptions();}
  initCoMssExtractor(setOfSelector);
  remove_satisfied = true;

  while(solve(activeBlockedClauses))
    {      
      vec<Lit> assums, identifiedCoMssClauses, approxMSS, approxMCS;
      for(int i = 0 ; i<setOfSelector.size() ; i++) 
        if(model[var(setOfSelector[i])] == l_False)
          approxMCS.push(setOfSelector[i]); else approxMSS.push(setOfSelector[i]);
      
      approxMCS.copyTo(assums);      
      
      // extract a coMSS
      while(assums.size())
        {
          Lit transitionFound = lit_Undef;
          bool assumsIsCoMSS = (assums.size() <= 1);
          
          if(optRefineApprox) assumsIsCoMSS = refineApproximationAll(approxMSS, approxMCS, assums);
          
          // select the next transition
          if(!assumsIsCoMSS)
            {
              selectTransition(assums);
              transitionFound = assums[0];
              assums[0] = assums.last();
              assums.pop();
            }         
          
          if(!assumsIsCoMSS && !solve(approxMSS, assums)) intersectConflict(assums, conflict);
          else
            {
              if(!assumsIsCoMSS){assums.clear(); assums.push(transitionFound);}
              assert(assums.size() <= approxMCS.size());
              
              // we add the transition constraint
              transferCoMSS(assums, approxMCS, identifiedCoMssClauses);
              if(optBackBone) backBoneAll(assums, approxMSS); // backbone method

              collectApproximationAll(approxMSS, approxMCS, assumsIsCoMSS); // collect the approximation
              
              // compute a new core with the current coMss approximation      
              if(approxMCS.size()) solve(approxMSS, approxMCS); else conflict.clear();              
              approxMCS.copyTo(assums);
                          
              intersectConflict(assums, conflict);
            }
        }// extraction finished
      
      ++nbRun;      
      cancelUntil(0);
      
      for(int i = 0 ; i<identifiedCoMssClauses.size() ; i++)
	{
	  if(selectorToIndex[var(identifiedCoMssClauses[i]) - nbInitVariable] == idx) return false;
	}
      addBlockedClause(identifiedCoMssClauses);
    }
    
  return true;
}// isNotInCoMSS



/**
   Iterate the processus of coMSS extraction in order to extract all
   the coMSS.   
*/
void Solver::enumAllCoMssBlocked()
{
  int nbRun = 0;
  
  // variable initialization
  if(verbosity > 0){printf("c All CoMSS extraction start ...\nc\n"); showOptions();}
  initCoMssExtractor(setOfSelector);
  remove_satisfied = true;

  while(solve(activeBlockedClauses))
    {      
      vec<Lit> assums, identifiedCoMssClauses, approxMSS, approxMCS;
      for(int i = 0 ; i<setOfSelector.size() ; i++) 
        if(model[var(setOfSelector[i])] == l_False)
          approxMCS.push(setOfSelector[i]); else approxMSS.push(setOfSelector[i]);
      
      approxMCS.copyTo(assums);      

      // extract a coMSS
      while(assums.size())
        {
          Lit transitionFound = lit_Undef;
          bool assumsIsCoMSS = (assums.size() <= 1);
          
          if(optRefineApprox) assumsIsCoMSS = refineApproximationAll(approxMSS, approxMCS, assums);
          
          // select the next transition
          if(!assumsIsCoMSS)
            {
              selectTransition(assums);
              transitionFound = assums[0];
              assums[0] = assums.last();
              assums.pop();
            }         
          
          if(!assumsIsCoMSS && !solve(approxMSS, assums)) intersectConflict(assums, conflict);
          else
            {
              if(!assumsIsCoMSS){assums.clear(); assums.push(transitionFound);}
              assert(assums.size() <= approxMCS.size());
              
              // we add the transition constraint
              transferCoMSS(assums, approxMCS, identifiedCoMssClauses);
              if(optBackBone) backBoneAll(assums, approxMSS); // backbone method

              collectApproximationAll(approxMSS, approxMCS, assumsIsCoMSS); // collect the approximation
              
              // compute a new core with the current coMss approximation      
              if(approxMCS.size()) solve(approxMSS, approxMCS); else conflict.clear();              
              approxMCS.copyTo(assums);
                          
              intersectConflict(assums, conflict);
            }
        }// extraction finished
      
      ++nbRun;      
      cancelUntil(0);
      if(verbosity > 0){ printf("c MCS(%d): ", nbRun); printIndexWrtSelectors(identifiedCoMssClauses);}
      if(optSaveMCS) saveOneMCS(identifiedCoMssClauses);
      // coMssVerification(identifiedCoMssClauses, setOfSelector);
      
      addBlockedClause(identifiedCoMssClauses);      
    }
    
  if(verbosity > 0) 
    {
      printInfoSolver();
      printf("c Number of visited coMSS: %d\n", nbRun);
      printf("s OPTIMUM FOUND\n");
    }
}// enumAllCoMssBlocked
    

 /**
    Verify if the assumption assums is a coMSS.
    @param[in] assums, the hypothetical coMss
 */
void Solver::coMssVerification(vec<Lit> &assums, vec<Lit> &setOfSelector)
{
  vec<Lit> tmp;

  optCoMssVerification = true;
      
  for(int i = 0 ; i<assums.size() ; i++) tmp.push(~assums[i]);
  for(int i = 0 ; i<setOfSelector.size() ; i++)
    {
      bool isIn = false;
      for(int j = 0 ; j<assums.size() && !isIn ; j++) isIn = (assums[j] == setOfSelector[i]);
      if(!isIn) tmp.push(setOfSelector[i]);
    }
      
  tmp.push(activeBlockedClauses);
  bool res = solve(tmp);
  if(!res) printf("It's not a coMSS approximation\n");
  assert(res == true);
  for(int i = 0 ; i<assums.size() ; i++)
    {
      tmp[i] = ~tmp[i];
      assert(solve(tmp) == false);
      tmp[i] = ~tmp[i];
    } 
  optCoMssVerification = false;
}// coMssVerification    

/**
   From a set of constraint obtained by model rotation we module the
   current coMSS to build new ones.

   @param[in] coMssFound, the current MCS found
   @param[in] productFree, the set of constraints obtained by model
   rotation (coMssFound[i] correspond to productFree[i])
 */
void Solver::computeNewCoMSS(vec<Lit> &coMssFound, vec< vec<Lit> > &productFree)
{
#if 1
  vec< vec<Lit> > setOfCoMssFound;
  setOfCoMssFound.push();
  coMssFound.copyTo(setOfCoMssFound.last());
  
  newDecisionLevel();
  for(int i = 0 ; i<coMssFound.size() ; i++)
    if(value(coMssFound[i]) == l_Undef) uncheckedEnqueue(~coMssFound[i]);
  propagate();

  // printf("coMSS: "); printIndexWrtSelectors(coMssFound);

  for(int i = 0 ; i<coMssFound.size() ; i++)
    {
      vec<Lit> &tmp = productFree[i];          
      for(int j = 0 ; j<tmp.size() ; j++)
        {
          if(value(tmp[j]) != l_Undef) continue;
          vec<Lit> mcsComputed;
          for(int k = 0 ; k<coMssFound.size() ; k++)
            {
              if(k == i) mcsComputed.push(tmp[j]);
              else mcsComputed.push(coMssFound[k]);
            }

          setOfCoMssFound.push();
          mcsComputed.copyTo(setOfCoMssFound.last());          
        }
    }
  cancelUntil(1);
  
  nbCoMssRotation += setOfCoMssFound.size() - 1;
  for(int i = 0 ; i<setOfCoMssFound.size() ; i++) addCoMssAndBlockIt(setOfCoMssFound[i]);
  cancelUntil(0);
#else
  printf("init: "); printIndexWrtSelectors(coMssFound);
  for(int i = 0 ; i<productFree.size() ; i++)
    {
      productFree[i].push(coMssFound[i]);
      printf("==> "); printIndexWrtSelectors(productFree[i]);
    }
  
  // make the cartesian product
  vec<Lit> tmpCoMss;
  vec< vec<Lit> > setOfCoMssFound;
  computeNewCoMSS_acc(tmpCoMss, productFree, setOfCoMssFound);
  assert(setOfCoMssFound.size());

  nbCoMssRotation += setOfCoMssFound.size() - 1;
  for(int i = 0 ; i<setOfCoMssFound.size() ; i++) addCoMssAndBlockIt(setOfCoMssFound[i]);
  cancelUntil(0);
  
  //exit(0);
#endif
}// computeNewCoMSS   


/**
   Realize the cartesian product (+ assure that the coMSS added was
   not already computed).
 */
void Solver::computeNewCoMSS_acc(vec<Lit> &currentCoMss, vec< vec<Lit> > &productFree, vec< vec<Lit> > &res)
{
  if(currentCoMss.size() == productFree.size() - 1)
    {
      vec<Lit> &lP = productFree.last();
      for(int i = 0 ; i<lP.size() ; i++)        
        {
          if(value(lP[i]) == l_True) continue;          
          
          res.push();
          currentCoMss.copyTo(res.last());
          (res.last()).push(lP[i]);
        }
    }else
    {
      assert(currentCoMss.size() < productFree.size());
      vec<Lit> &lP = productFree[currentCoMss.size()];
      for(int i = 0 ; i<lP.size() ; i++)
        {
          if(value(lP[i]) == l_True) continue;
          
          newDecisionLevel();
          if(value(lP[i]) == l_Undef) uncheckedEnqueue(~lP[i]);
          propagate();
          currentCoMss.push(lP[i]);
          computeNewCoMSS_acc(currentCoMss, productFree, res);
          currentCoMss.pop();
          cancelUntil(decisionLevel() - 1);
        }
    }
}// computeNewCoMSS_acc


/**
   Take all the set and realize the intersection
 */
void Solver::makeCoreIntersection(vec<coreWrtLit> &coreList)
{
  for(int i = nbInitVariable ; i<nVars() ; i++) intersectHit[i] = 0;
  for(int i = 0 ; i<coreList.size() ; i++)
    {          
      vec<Lit> &lst = coreList[i].core;         
      for(int j = 0 ; j<lst.size() ; j++) intersectHit[var(lst[j])]++;
    } 
}// makeCoreIntersection


/**
   Collect the selector which must be transfer to the identified coMss
   part.
 */
void Solver::transferAndCollect(Lit l, vec<Lit> &ccore, vec<Lit> &aCoMss, vec<Lit> &idenCoMss,
                        vec<coreWrtLit> &collectedCore)
{
  // we collect the propagated identified coMss
  int i, j;
  for(i = j = 0 ; i<aCoMss.size() ; i++)
    {          
      if(var(aCoMss[i]) == var(l) || value(aCoMss[i]) == l_False)
        {
          idenCoMss.push(aCoMss[i]);
          collectedCore.push();
          (collectedCore.last()).l = aCoMss[i];
              
          // we visit the implication graph to extract the core
          if(var(aCoMss[i]) == var(l)) ccore.copyTo((collectedCore.last()).core);
          else
            {         
              recursiveAnalyzeFinal(aCoMss[i]);
              recCollectedCores[var(aCoMss[i])].copyTo((collectedCore.last()).core);
              ((collectedCore.last()).core).push(aCoMss[i]);
            }
        }else aCoMss[j++] = aCoMss[i];
    }
  aCoMss.shrink(i - j);
}// transferAndCollect


/**
   Make the recursive rotation.
   
   @param[in] rlSolver, use the datat structure from local search (flipatom)
   @param[in] coMssFound, the coMss found
   @param[in] ccore, the set of associated core

   \return false (not use for the moment)
 */
bool Solver::rotationProcessus(SolverRL &rlSolver, vec<Lit> &coMss, vec<coreWrtLit> &ccore)
{ 
  assert(ccore.size() == coMss.size());
  cancelUntil(0);
  vec< vec<Lit> > productFree;
  
  // make the core intersection
  makeCoreIntersection(ccore);  
  
  // make the rotation
  vec<int> indexCoMssFound;
  vec<int> szCores;
  int maxSize = 0;

  for(int i = 0 ; i<coMss.size() ; i++)
    {
      indexCoMssFound.push(mapSelectorToIndex[var(coMss[i]) - nbInitVariable]);
      vec<Lit> &tmp = ccore[i].core;
      
      int j,k;
      for(j = k = 0 ; j<tmp.size() ; j++)        
        if((var(tmp[j]) != var(coMss[i])) && intersectHit[var(tmp[j])] <= 1) tmp[k++] = tmp[j];
      tmp.shrink_(j - k);
      szCores.push(tmp.size());
      
      if(tmp.size() > maxSize) maxSize = tmp.size();
#if 0
      if(tmp.size()) 
        {
          printf("--> %d\n", selectorToIndex[var(coMss[i]) - nbInitVariable]);
          for(int j = 0 ; j<tmp.size() ; j++)
            {
              showClause(ca[clauses[mapSelectorToIndex[var(tmp[j]) - nbInitVariable]]]);
            }
          printf("\n");
          // exit(0);
          getchar();
        }
#endif
      //printf("%d ", tmp.size());
    }
  //printf("\n");
  //getchar();
  
  if(maxSize || coMss.size() == 1)
    {      
      newDecisionLevel();
      int start = trail.size();
      uncheckedEnqueue(activeBlockedClauses);
      propagate();
      
      for(int i = start ; i<trail.size() ; i++) canBeSelected[var(trail[i])] = false;      
      rlSolver.makeRotation(clauses, ca, indexCoMssFound, coMss, productFree, canBeSelected, szCores, currAss);
      for(int i = start ; i<trail.size() ; i++) canBeSelected[var(trail[i])] = true;
      
      assert(coMss.size() == productFree.size());        

      for(int i = 0 ; i<productFree.size() ; i++) 
        {      
          vec<Lit> &tmp = productFree[i];
          
          int j,k;
          for(j = k = 0 ; j<tmp.size() ; j++)
            {
              assert(value(tmp[j]) == l_Undef);
              if(intersectHit[var(tmp[j])] <= 1) tmp[k++] = tmp[j]; 
            }
          tmp.shrink_(j - k);
          //printf("%d ", tmp.size());
        }
      //printf("\n");
      //getchar();
      
      computeNewCoMSS(coMss, productFree);
    }else
    {
      cancelUntil(0);
      ++nbComputedMCS;
      printf("c MCS(%d/%d): ", nbComputedMCS, nbCoMssRotation); printIndexWrtSelectors(coMss);
      // coMssVerification(coMss, setOfSelector);
      addBlockedClause(coMss);
    }

  if(nbCoMssRotation > 1000 && !nbCoMssRotation) optRotation = false;
  return false;
}// rotationProcessus


/**
   Iterate the coMSS processus extraction in order to extract all the
   coMSS.
*/
void Solver::enumAllCoMssTree()
{  
  vec<Lit> prevConflict, assums, identifiedMCS, approxMSS, approxMCS;
  vec<coreWrtLit> saveCores;  
 
  // variable initialization
  printf("c All CoMSS extraction start ...\nc\n"); showOptions();
  initCoMssExtractor(setOfSelector);

  // used to perform model rotation    
  SolverRL rlSolver(nVars() + 1, nbInitVariable); 
  rlSolver.initWithClauses(clauses, ca);

  while(solve(activeBlockedClauses))
    {
      assert(decisionLevel() == 0);                  
      initializeProcedure(setOfSelector, approxMSS, approxMCS, assums, identifiedMCS, prevConflict, saveCores);

#if 0
      printf("Initialization: \n");
      printf("approxMCS: "); showList(approxMCS);
      // printf("approxMSS: "); showList(approxMSS);
      printf("assums: "); showList(assums);
      printf("identifiedMCS: "); showList(identifiedMCS);
#endif

      // extract a coMSS
      while(assums.size())
        {
          Lit transitionFound = lit_Undef;
          bool assumsIsCoMSS = assums.size() <= 1;
                  
          // select the next transition
          selectTransition(assums);
          transitionFound = assums[0];
          assums[0] = assums.last();
          assums.pop();

          if(assums.size() && !solve(approxMSS, assums))
            {
              intersectConflict(assums, conflict);
              conflict.copyTo(prevConflict);
            }
          else
            {
              assert(transitionFound != lit_Undef);
              
              vec<Lit> detected;
              transferAndCollect(transitionFound, prevConflict, approxMCS, detected, saveCores);
              for(int i = 0 ; i<detected.size() ; i++) identifiedMCS.push(detected[i]); 

              collectApproximationAll(approxMSS, approxMCS, assumsIsCoMSS); // collect the approximation
              
              // compute a new core with the current coMss approximation      
              if(approxMCS.size()) solve(approxMSS, approxMCS); else conflict.clear();              

              approxMCS.copyTo(assums);              
              intersectConflict(assums, conflict);
              conflict.copyTo(prevConflict);              
            }
        }// extraction finished
      
      if(!optRotation)
        {
          cancelUntil(0);
          ++nbComputedMCS;
          printf("c MCS(%d): ", nbComputedMCS); printIndexWrtSelectors(identifiedMCS);
          // coMssVerification(identifiedMCS, setOfSelector);
          addBlockedClause(identifiedMCS);
        }else if(rotationProcessus(rlSolver, identifiedMCS, saveCores)) break;              
      //if(nbComputedMCS > 10800) break;
    }
  
  printInfoSolver();    
  printf("c\nc Number of coMSS obtained by rotation: %d\n", nbCoMssRotation);
  printf("c Number of clauses identified by a core: %d\n", nbIdentifiedByCore);
  printf("c Number of flips: %lld\n", rlSolver.getNumFlip());
  printf("c Number of visited coMSS: %d\n", nbComputedMCS);
  printf("s OPTIMUM FOUND\n");
}// enumAllCoMssTree


/**
   Return a set of clauses from approxCoMss which is a "coMSS" with
   respect to the set of unit clauses of the problem

   @param[in] approxCoMss, the current coMSS approximation
*/
void Solver::constructive(vec<Lit> &approxCoMss)
{
  printf("c The method used to detect the coMSS is the constructive approach\n");

  for(int i = 0 ; i<approxCoMss.size() ; i++)
    {
      if(value(approxCoMss[i]) != l_Undef) continue;
      if(solve(approxCoMss[i])) 
        {
          printf("c | SAT | %8.2lf | %8d | %8d\n", cpuTime(), i, approxCoMss.size());
          if(value(approxCoMss[i]) == l_Undef) uncheckedEnqueue(approxCoMss[i]);
          
          // we remove the satisfiable clauses
          for(int j = i + 1 ; j<approxCoMss.size() ; j++)
            {
              if(value(approxCoMss[j]) != l_Undef) continue;
              
              Clause &c = ca[clauses[mapSelectorToIndex[var(approxCoMss[j]) - nbInitVariable]]];
              bool isSAT = false;                     
              for(int k = 0 ; k<c.size() && !isSAT ; k++) isSAT = value(c[k]) == l_True;
              if(isSAT) uncheckedEnqueue(approxCoMss[j]);                
            }
        }
      else
        {         
          if(value(approxCoMss[i]) == l_Undef) uncheckedEnqueue(~approxCoMss[i]);          
          if(optBackBone) backBone(var(approxCoMss[i])); // backbone method proposed by joao
          printf("c | UNS | %8.2lf | %8d | %8d\n", cpuTime(), i, approxCoMss.size());
        }
    }
}// constructive


/**
   Return a set of clauses from approxCoMss which is a "coMSS" with
   respect to the set of unit clauses of the problem

   @param[in] approxCoMss, the current coMSS approximation
   @param[out] coMss, subset of approximation belonging to the coMSS
 */
void Solver::recursiveConflict(vec<Lit> &approxCoMss)
{
  printf("c The method used to detect the coMSS is destructive using the final\n");
  vec<Lit> assums;
  approxCoMss.copyTo(assums);

  while(assums.size())
    {
      assums[0] = ~assums[0];
      if(assums.size() > 1 && !solve(assums))
        {
          putNegationList(conflict, assums);
          printf("c | UNS | %8.2lf | %8d | %8d\n", cpuTime(), assums.size(), approxCoMss.size());
        }
      else
        {
          if(value(assums[0]) == l_Undef) uncheckedEnqueue(assums[0]); // transition clause            
          if(optBackBone) backBone(var(assums[0])); // backbone method proposed by joao
          collectApproximation(approxCoMss, assums.size() == 1); // collect the approximation

          solve(approxCoMss);
          putNegationList(conflict, assums);
          printf("c | SAT | %8.2lf | %8d | %8d\n", cpuTime(), assums.size(), approxCoMss.size());
        }
    }  

  for(int i = 0 ; i<approxCoMss.size() ; i++) uncheckedEnqueue(approxCoMss[i]);
}// recursiveConflict



/**
   Return a set of clauses from approxCoMss which is a "coMSS" with
   respect to the set of unit clauses of the problem

   @param[in] approxCoMss, the current coMSS approximation
   @param[out] coMss, subset of approximation belonging to the coMSS
 */

void Solver::recursiveConflictAndNegation(vec<Lit> &approxCoMss)
{
  if (verbosity > 0) printf("c Recursive coMSS extraction\n");
  
  int initSzAssums = 0;
  vec<Lit> assums;
  approxCoMss.copyTo(assums);

  while(assums.size())
    {
      bool assumsIsCoMSS = (assums.size() < 1);
      if(optRefineApprox)
        {
          assert(trail.size() >= qhead);      
          assumsIsCoMSS = refineApproximation(assums);     
          assert(trail.size() >= qhead);      
        }
      if(!assumsIsCoMSS)
        {
          selectTransition(assums);
          assums[0] = ~assums[0];
          initSzAssums = (optAddNegation) ? assums.size() : addNegationClause(assums[0], assums);
        }      
      
      if(!assumsIsCoMSS && !solve(assums))
        {
          if(onlyContainsSelector(conflict)) putNegationList(conflict, assums);                        
          else
            {      
              if(onlyContainsInitVariable(conflict)) uncheckedEnqueue(~assums[0]);
              else
                {
                  conflict.push(~assums[0]);
                  createNewClause(blockedClauses, conflict);
                }
              
              assums.shrink_(assums.size() - initSzAssums);
              assums[0] = assums.last();
              assums.pop();
            }
          cancelUntil(0);
          if (verbosity > 0) printf("c | UNS | %8.2lf | %8d | %8d\n", cpuTime(), assums.size(), approxCoMss.size());
        } else
        { 
          assert(trail.size() >= qhead);
          if(assumsIsCoMSS)
            {       
              for(int i = 0 ; i<assums.size() ; i++)
                {
                  if(value(assums[i]) == l_Undef) uncheckedEnqueue(~assums[i]);
                  if(optBackBone) backBone(var(assums[i])); // backbone method proposed by joao
                }
            }else
            {
              if(value(assums[0]) == l_Undef) uncheckedEnqueue(assums[0]); // transition clause          
              if(optBackBone) backBone(var(assums[0])); // backbone method proposed by joao                
            }
                   
          collectApproximation(approxCoMss, assumsIsCoMSS); // collect the approximation        
          
          // compute a new core with the current coMss approximation
          solve(approxCoMss);

          putNegationList(conflict, assums);  
          if (verbosity > 0) printf("c | SAT | %8.2lf | %8d | %8d\n", cpuTime(), assums.size(), approxCoMss.size());
        }
    }

  for(int i = 0 ; i<approxCoMss.size() ; i++)
    if(value(approxCoMss[i]) == l_Undef) uncheckedEnqueue(approxCoMss[i]); // do not appear in the coMSS
}// recursiveConflictAndClauseNegation



/**
   Return a set of clauses from approxCoMss which is a "coMSS" with
   respect to the set of unit clauses of the problem

   @param[in] approxCoMss, the current coMSS approximation
 */
void Solver::destructiveAndNegation(vec<Lit> &approxCoMss)
{
  printf("c The method used to detect the coMSS is destructive with clause negation\n");

  int pos = approxCoMss.size();
  while(approxCoMss.size())
    {        
      if(pos < approxCoMss.size()) approxCoMss[pos] = ~approxCoMss[pos];
      
      vec<Lit> assums;
      approxCoMss.copyTo(assums);
      if(pos < approxCoMss.size())
        {
          Clause &c = ca[clauses[mapSelectorToIndex[var(approxCoMss[pos]) - nbInitVariable]]];
          for(int i = 0 ; pos < approxCoMss.size() && i<c.size() ; i++)
            if(value(c[i]) == l_Undef && var(c[i]) < nbInitVariable) assums.push(~c[i]); 
        }
  
      if(!solve(assums))
        {
          // collect literal which must be assign ...
          int i, j, save = -1;
          bool restart = false;

          for(pos = i = j = 0 ; i<approxCoMss.size() ; i++) 
            {
              if(value(approxCoMss[i]) == l_Undef)
                {
                  if(var(approxCoMss[i]) == var(conflict[0])) save = j;
                  approxCoMss[j++] = approxCoMss[i];              
                  if(restart && sign(approxCoMss[j - 1])) approxCoMss[j - 1] = ~approxCoMss[j - 1];
                  if(!sign(approxCoMss[j - 1])) pos = j - 1;
                }else restart = restart || !sign(approxCoMss[i]);
            }
          approxCoMss.shrink_(i - j);

          if(save != -1)
            {
              Lit l = approxCoMss[save];
              approxCoMss[save] = approxCoMss[pos];
              approxCoMss[pos] = l;
            }

          if(restart) pos++;
          printf("c | UNS | %8.2lf | %8d | %8d\n", cpuTime(), assums.size(), approxCoMss.size());
        }
      else
        {
          for(int i = 0 ; i<pos ; i++)
            {
              assert(!sign(approxCoMss[i]));
              uncheckedEnqueue(approxCoMss[i]);
            }
          if(pos == approxCoMss.size()) break;

          if(value(approxCoMss[pos]) == l_Undef) uncheckedEnqueue(approxCoMss[pos]); // transition
          if(optBackBone) backBone(var(approxCoMss[pos])); // backbone method proposed by joao
          
          for(int i = ++pos ; i<approxCoMss.size() ; i++) approxCoMss[i - pos] = approxCoMss[i];
          approxCoMss.shrink_(pos);
          for(int i = 0 ; i<approxCoMss.size() ; i++) approxCoMss[i] = ~approxCoMss[i];
          
          pos = approxCoMss.size();
          printf("c | SAT | %8.2lf | %8d | %8d\n", cpuTime(), assums.size(), approxCoMss.size());
        }
    }
}// destructiveAndNegation
