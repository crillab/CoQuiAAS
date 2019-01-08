#include "cmp/approx/bCoMSSApprox.h"

using namespace CMP;

Basic_CoMSSApprox::Basic_CoMSSApprox(SatSolver* _s, vec<Lit>& _select, vector<int>& _map) : 
  CoMSSApprox(_s,  _select, _map) {}//constructor

void Basic_CoMSSApprox::operator()(vec<Lit>& mss, vec<Lit>& coMss)
{
  //printf("c Basic approximation\n");
  int lim_restart = slv->nAssigns();
  for(int i=0; i<select.size(); i++) {
    if(slv->originalVar(var(select[i])) && (slv->value(select[i])==l_Undef)) slv->uncheckedEnqueue(select[i]);
  }
  
  int res = slv->solve(conf_lim);
  vec<lbool> assigns;  

//TODO
//check if res == sat or unsat befor call modelValue()
  if(res) {for(int i = 0 ; i<slv->nVars(); i++) slv->setPolarity(i, (slv->modelValue(i) != l_True));}
  
  
  for(int i=0; i<slv->nVars(); i++) {
    assigns.push();
    assigns.last() = (slv->getPolarity(i) != 0)? l_False : l_True; //TODO
  }
  
  //================Local Search=========================
  if(local_search) { //refine approximation
    // LocalSearch rlSolver(slv->nVars()+1, slv->nbInitVariable);
    // rlSolver.initWithClauses(slv->clauses, slv->ca);
    // res = rlSolver.refineAssigns((local_search==2), slv->clauses, slv->ca, assigns);

    //for(int i = 0; i<trail.size() && res; i++) res = (assignments[var(trail[i])] ^ sign(trail[i])) == l_True;
    // for(int i = 0; i<activeUnitLits.size() && res; i++) {
    //   res = (assignments[var(activeUnitLits[i].l)] ^ sign(activeUnitLits[i].l)) == l_True;
    // }
    //for(int i = 0; i<slv->nVars(); i++) slv->setPolarity(i, (assigns[i] != l_True));
  }
  //
  slv->restartUntil(lim_restart);
    
  for(int i=0; i<select.size(); i++) {
    if(slv->originalVar(var(select[i]))) {
      if((assigns[var(select[i])] ^ sign(select[i])) == l_True) mss.push(select[i]);
      else coMss.push(select[i]);
    }
    else {
      int id = sel2cl(select[i]);
      Clause& c =  slv->getIth_clauses(id);
      bool actived = false;
      for(int j=0; j<c.size() && !actived; j++) {
	if(!slv->originalVar(var(c[j]))) continue;
	actived = ((assigns[var(c[j])] ^ sign(c[j])) == l_True);
      }
      if(actived) mss.push(select[i]);
      else coMss.push(select[i]);
    }
  }
}//search
