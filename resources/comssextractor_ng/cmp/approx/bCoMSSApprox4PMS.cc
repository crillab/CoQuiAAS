#include "cmp/approx/bCoMSSApprox4PMS.h"

using namespace CMP;

Basic_CoMSSApprox4PMS::Basic_CoMSSApprox4PMS(SatSolver* _s, vec<Lit>& _select, vector<int>& _map) : 
  CoMSSApprox(_s,  _select, _map) {}//constructor

void Basic_CoMSSApprox4PMS::operator()(vec<Lit>& mss, vec<Lit>& coMss)
{
  slv->restartUntil(slv->nAssigns());
  for(int i = 0 ; i<slv->nVars(); i++) slv->setPolarity(i, (slv->modelValue(i) != l_True));

  for(int i=0; i<select.size(); i++) {
    if(slv->originalVar(var(select[i]))) {
      if(slv->modelValue(select[i]) == l_True) mss.push(select[i]);
      else coMss.push(select[i]);
    }
    else {
      int id = sel2cl(select[i]);
      Clause& c =  slv->getIth_clauses(id);
      bool actived = false;
      for(int j=0; j<c.size() && !actived; j++) {
	if(!slv->originalVar(var(c[j]))) continue;
	actived = (slv->modelValue(c[j]) == l_True);
      }
      if(actived) mss.push(select[i]);
      else coMss.push(select[i]);
    }
  }
}//search
