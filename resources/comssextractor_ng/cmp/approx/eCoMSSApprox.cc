#include "cmp/approx/eCoMSSApprox.h"

using namespace CMP;

Enhanced_CoMSSApprox::Enhanced_CoMSSApprox(SatSolver* _s, vec<Lit>& _select, vector<int>& _map) : 
  CoMSSApprox(_s,  _select, _map) { }//constructor

void Enhanced_CoMSSApprox::operator()(vec<Lit>& mss, vec<Lit>& coMss)
{
  //int limit_restart = slv->nAssigns();
   
  vec<lbool> assigns;
  for(int i=0; i<slv->nVars(); i++) assigns.push(l_False);

  vec<Lit> unary_cls;
  //vec<CRef> relaxed_cls;
  for(int i=0; i<select.size(); i++) {
    if(slv->originalVar(var(select[i]))) { //var(select[i]) < slv->nbInitVariable
      unary_cls.push(select[i]);
      if((assigns[var(select[i])] ^ sign(select[i])) == l_True) {
	if (slv->value(select[i]) == l_Undef) slv->uncheckedEnqueue(select[i]);
      }
    }
    else if (slv->value(select[i]) == l_Undef) slv->uncheckedEnqueue(select[i]);
  }
  assigns.clear();
 
  int res = slv->solve();
  /*
//TODO 
//check if sat or unsat !!! before call modelValue()
  for(int i=0; i<slv->nVars(); i++) slv->setPolarity(i, slv->modelValue(i) != l_True);
  for(int i=0; i<slv->nVars(); i++) {
    assigns.push();
    assigns.last() = (slv->polarity[i] != 0)? l_False : l_True;
  }
  */

  //TODO local search
  //
 
  //slv->restartUntil(limit_restart);
  slv->restartUntil(0);
  
  for(int i=0; i<select.size(); i++) {
    if(slv->originalVar(var(select[i]))) continue;
    if(res) {mss.push(select[i]); continue;}
    //
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

  for(int i=0; i<unary_cls.size(); i++) {
    if((assigns[var(unary_cls[i])] ^ sign(unary_cls[i])) == l_True) mss.push(unary_cls[i]);
    else coMss.push(unary_cls[i]);
  }
}//operator()
