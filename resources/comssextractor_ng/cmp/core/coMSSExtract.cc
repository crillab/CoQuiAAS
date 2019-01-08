#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>

#include "cmp/core/coMSSExtract.h"

using namespace CMP;

CoMSSExtract::CoMSSExtract(WCNF &f): formula(f), sfact(MINISAT), slv(&sfact.instance()), 
				     mapOrgID2in(2*f.nClauses(), NULL), orgSelector_map(2*f.nVars(), NULL)
{
  slv->initNbVariable(f.nVars() - 1);
  
  //add hard clauses 
  for(int i=0; i<f.nHards(); i++) {
    vec<Lit> ps; f.getHard(i, ps);
    if(!slv->addClause(ps)) {printf("Hard part are unsat!\n"); exit(30);}
  }
  
  if(f.nHards()) {/*partial maxsat*/ 
    int limitRestart = slv->nAssigns();
    int res = slv->solve();
    if(!res) {printf("Hard part are unsat!\n"); exit(30);}
    slv->restartUntil(limitRestart);
  }

  //add soft clauses
  for(int i=0 ; i<f.nSofts(); i++)
    {
      vec<Lit> ps;
      f.getSoft(i, ps);
      if(ps.size() == 1)
        {
          if(mapIdxClause.find(toInt(ps[0])) == mapIdxClause.end()) selectors.push(ps[0]);
          mapIdxClause[toInt(ps[0])].push_back(f.getSoft_ID(i));
	  mapOrgID2in[f.getSoft_ID(i)] = new int(toInt(ps[0]));
	  if(!orgSelector_map[toInt(ps[0])]) orgSelector_map[toInt(ps[0])] = new int(i);
        }else
        {
	  // Check if clause is satisfied and remove false/duplicate literals:
	  sort(ps);
	  bool satisfied = false;
	  Lit p; int k, j;
	  for (k = j = 0, p = lit_Undef; j < ps.size() && !satisfied; j++)
	    {      
	      if (slv->value(ps[j]) == l_True || ps[j] == ~p) satisfied = true;
	      else if (slv->value(ps[j]) != l_False && ps[j] != p) ps[k++] = p = ps[j];
	    }
	  
	  if(!satisfied) {
	    ps.shrink(j - k);
	    switch(ps.size()) {
	    case 0:
	      if(mapIdxClause.find(toInt(f.getSoft(i)[0])) == mapIdxClause.end()) selectors.push(f.getSoft(i)[0]);
	      mapIdxClause[toInt(f.getSoft(i)[0])].push_back(f.getSoft_ID(i));
	      mapOrgID2in[f.getSoft_ID(i)] = new int(toInt(f.getSoft(i)[0]));
	      if(!orgSelector_map[toInt(f.getSoft(i)[0])]) orgSelector_map[toInt(f.getSoft(i)[0])] = new int(i);
	      break;
	    case 1:
	      if(mapIdxClause.find(toInt(ps[0])) == mapIdxClause.end()) selectors.push(ps[0]);
	      mapIdxClause[toInt(ps[0])].push_back(f.getSoft_ID(i));
	      mapOrgID2in[f.getSoft_ID(i)] = new int(toInt(ps[0]));
	      if(!orgSelector_map[toInt(ps[0])]) orgSelector_map[toInt(ps[0])] = new int(i);
	      break;
	    default:
	      //creates new selector
	      ps.clear();
	      f.getSoft(i, ps);
	      Var v = slv->newVar(false, true);
	      Lit ls = mkLit(v, true);
	      selectors.push(~ls);
	      ps.push(ls);
      	      slv->addClause(ps);
	      mapIdxClause[toInt(~ls)].push_back(f.getSoft_ID(i));
	      mapIdxClauseInSolver.push_back(slv->nClauses() - 1);
	      mapOrgID2in[f.getSoft_ID(i)] = new int(toInt(~ls));
	      //
	      freshSelector_map.push_back(i);
	    }
	  }
        }
    }
  
  while(markedSelector.size() < slv->nVars()) markedSelector.push(0);

  optBackbone  = false;
  optClauseD   = false;
  optClauseN   = false;
  verb = 0;
}// CoMSSExtract

CoMSSExtract::~CoMSSExtract()
{
  free(slv);
}

void CoMSSExtract::operator()(vec<Lit> &coMss)
{
  coMss.clear();
  vec<Lit> mss, unknown;
  CoMSSApprox* approx;

  switch(optApprox) {
  case 1:
    if(pmsFormula()) approx = new Basic_CoMSSApprox4PMS(slv, selectors, mapIdxClauseInSolver);
    else approx = new Basic_CoMSSApprox(slv, selectors, mapIdxClauseInSolver);
    //=================================
    approx->conf_lim = conf_lim;
    approx->local_search = local_search;
    //=================================
    (*approx)(mss, coMss);
    break;
  case 2:
    if(pmsFormula()) approx = new Enhanced_CoMSSApprox4PMS(slv, selectors, mapIdxClauseInSolver);
    else approx = new Enhanced_CoMSSApprox(slv, selectors, mapIdxClauseInSolver);
    (*approx)(mss, coMss);
    break;
  default :
    selectors.copyTo(coMss);
  }

  for(int i = 0 ; i<markedSelector.size() ; i++) markedSelector[i] = 0;
  for(int i = 0; i<mss.size(); i++) markedSelector[var(mss[i])] = sign(mss[i]) + 1;
  for(int i = 0; i<coMss.size(); i++) if(markedSelector[var(coMss[i])] != sign(coMss[i]) + 1) unknown.push(coMss[i]);
  coMss.clear();

  if(verb>1) {
    printf("c #MSS : %d\n", mss.size());
    printf("c #coMSS : %d\n", coMss.size());
    printf("c #unknown : %d\n", unknown.size());
  }

  search(mss, unknown, coMss);
  if (verb > 1) printStats();
}



void CoMSSExtract::operator()(vec<Lit>& mss, vec<Lit>& coMss)
{
  vec<Lit> unkn, tmp_unkn;
  for(int i = 0; i<markedSelector.size(); i++) markedSelector[i] = 0;
  for(int i = 0; i<mss.size(); i++) {
    markedSelector[var(mss[i])] = sign(mss[i]) + 1;
    if(slv->originalVar(var(mss[i])) && isSelector(~mss[i])) tmp_unkn.push(~mss[i]);
  }
  // printf("mss: ");printLits(mss);
  // printf("coMss: ");printLits(coMss);
  // printf("tmp_unkn: ");printLits(tmp_unkn);
  
  for(int i = 0; i<coMss.size(); i++) {
    if(!markedSelector[var(coMss[i])]) {
      markedSelector[var(coMss[i])] = sign(~coMss[i]) + 1;
      if(slv->originalVar(var(coMss[i])) && isSelector(~coMss[i])) unkn.push(~coMss[i]);
    } else {
      for(int j=0; j<tmp_unkn.size(); j++) {
	if(tmp_unkn[j] == coMss[i]) {
	  tmp_unkn[j] = tmp_unkn.last();
	  tmp_unkn.pop();
	  break;
	}
      }
    }
  }
 
  for(int i=0; i<tmp_unkn.size(); i++) unkn.push(tmp_unkn[i]);
  for(int i=0; i<selectors.size(); i++) if(!markedSelector[var(selectors[i])]) unkn.push(selectors[i]);
  
  if(verb>1) {
    printf("c #MSS : %d\n", mss.size());
    printf("c #coMSS : %d\n", coMss.size());
    printf("c #unknown : %d\n", unkn.size());
  }

  assert((selectors.size() - unkn.size()) == (mss.size() + coMss.size()));

  search(mss, unkn, coMss);
}


/**
   Remove the satisfied literals (version to enumerate all the MCS)
*/
void CoMSSExtract::shrinkUnknown(vec<Lit> &approxMSS, vec<Lit> &approxCoMSS, vec<Lit> &unknown, bool isCoMss)
{
  int i, j;
  // we remove the satisfiable clauses
  for(i = j = 0 ; i<unknown.size() ; i++)
    {
      Lit l = unknown[i];
      if(markedSelector[var(l)])
        {
          if(markedSelector[var(l)] != sign(l) + 1) approxCoMSS.push(l);
	  else approxMSS.push(l);
          continue;
        }
      
      if(isCoMss) {unknown[j++] = l; continue;}
      bool isSAT = false;
      if(slv->originalVar(var(l))) isSAT = (slv->modelValue(l) == l_True);
      else
        {
          Clause &c = slv->getIth_clauses(sel2cl(l));
          for(int k = 0 ; k<c.size() && !isSAT ; k++) isSAT = (var(c[k]) != var(l)) && (slv->modelValue(c[k]) == l_True);
        }
              
      if(!isSAT) unknown[j++] = l;
      else
        {
          markedSelector[var(l)] = sign(l) + 1;
          approxMSS.push(l);
        }
    }  
  unknown.shrink(i - j);
}//shrinkUnknown


/**
   Add the negation of the clause pointed by sel.
   @param[in] sel, the selector
*/
void CoMSSExtract::backbone(Lit sel)
{
  if(slv->originalVar(var(sel))) return;
  Clause &c = slv->getIth_clauses(sel2cl(sel));;      
  for(int i = 0 ; i<c.size() ; i++) {
    if((var(c[i]) != var(sel)) && (slv->value(c[i]) == l_Undef)) slv->uncheckedEnqueue(~c[i]); 
  }
}// backbone


int CoMSSExtract::sel2cl(Lit p) {
  assert(!slv->originalVar(var(p)));
  return mapIdxClauseInSolver[var(p) - formula.nVars()];
}//sel2cl


bool CoMSSExtract::isSelector(Lit p) {
  assert(var(p)<slv->nVars());
  if(slv->originalVar(var(p))) return (orgSelector_map[toInt(p)] != NULL);
  return true;
}//isSelector

// selectorToIdxClause
void CoMSSExtract::in2ex(vec<Lit> &cs, vec<int> &out)
{
  out.clear();
  for(int i = 0 ; i<cs.size() ; i++)
    {
      assert(mapIdxClause.find(toInt(cs[i])) != mapIdxClause.end());
      for(unsigned int j = 0 ; j<mapIdxClause[toInt(cs[i])].size() ; j++) out.push(mapIdxClause[toInt(cs[i])][j]); 
    }
}//in2ex
  
vector<Lit> CoMSSExtract::ex2in(vector<int> idCls)
{
  vector<Lit> out;
  for(size_t i=0; i<idCls.size(); i++) {
    assert(mapOrgID2in[idCls[i]] != NULL);
    out.push_back(toLit(*mapOrgID2in[idCls[i]]));
  }
  return out;
}//ex2in


 /**
    Verify if the given vector is a coMSS
    @param[in] coMSS, the hypothetical coMss
 */
void CoMSSExtract::check(vec<Lit> &coMSS)
{
}// check
