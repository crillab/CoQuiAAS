#include "cmp/approx/eCoMSSApprox4PMS.h"

using namespace CMP;

Enhanced_CoMSSApprox4PMS::Enhanced_CoMSSApprox4PMS(SatSolver* _s, vec<Lit>& _select, vector<int>& _map) : 
  CoMSSApprox(_s, _select, _map) {}//constructor

void Enhanced_CoMSSApprox4PMS::operator()(vec<Lit>& mss, vec<Lit>& coMss)
{
  slv->restartUntil(slv->nAssigns());
  vec<Lit> unary_cls;
  for(int i=0; i<select.size(); i++) {
    if(slv->originalVar(var(select[i]))) unary_cls.push(select[i]);
  }

  //  assert(slv->model.size());
  vec<lbool> assigns;
  for(int i=0; i<slv->nVars(); i++) assigns.push(slv->modelValue(i));

  //TODO local search

  //adjust polarity
  for(int i=0; i<slv->nVars(); i++) slv->setPolarity(i, assigns[i] != l_True);

  for(int i=0; i<unary_cls.size(); i++) {
    if((assigns[var(unary_cls[i])] ^ sign(unary_cls[i])) == l_True) mss.push(unary_cls[i]);
    else coMss.push(unary_cls[i]);
  }

  for(int i=0; i<select.size(); i++) {
    if(slv->originalVar(var(select[i]))) continue;
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
}//search
