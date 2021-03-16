#include "cmp/enum/ModelRotator.h"

#define _print_ 0

using namespace CMP;

ModelRotator::ModelRotator(WCNF& wcnf, vector<int*>& orgSelector, vector<int>& freshSelector, vector<int*>& orgID2sel)
  : formula(wcnf), orgSelector_map(orgSelector), freshSelector_map(freshSelector),
    orgID2sel_map(orgID2sel), s_occurs(2*wcnf.nVars(), vector<int>(0)), 
    h_occurs(2*wcnf.nVars(), vector<int>(0)), visited(wcnf.nHards() + wcnf.nSofts(), 0),
    visited_var(wcnf.nVars(), false), cand(wcnf.nSofts(), 0), actived(wcnf.nSofts(), 0),
    uns_sft(0), uns_hrd(0), singletonSft(false), singletonHrd(false), stamp_cand(0), stamp_core(0)
{
  for(int i=0; i<formula.nHards(); i++) {
    const vector<Lit>& cl = formula.getHard(i);
    for(size_t j=0; j<cl.size(); j++) {
      h_occurs[toInt(cl[j])].push_back(i);
    }
  }
  for(int i=0; i<formula.nSofts(); i++) {
    vector<Lit> soft_cls = formula.getSoft(i);
    for(size_t j=0; j<soft_cls.size(); j++) {
      s_occurs[toInt(soft_cls[j])].push_back(i);
    }
  }
}//ModelRotator


//ModelRotator::~ModelRotator() {}
void ModelRotator::process(RotateModel& rm, bool reset)
{
  if(reset) setup(rm);  
  
  vector<Lit> clT;
  if(singletonSft) {   
    if(cand[uns_sft] == stamp_cand) {rm.crits.push_back(formula.getSoft_ID(uns_sft)); }
    visited[uns_sft] = stamp_cand;
    clT = formula.getSoft(uns_sft);
  } else {
    assert(singletonHrd);
    if(visited[uns_hrd] == stamp_cand) return;
    visited[uns_hrd] = stamp_cand;
    clT = formula.getHard(uns_hrd);
  }  
  
  for(size_t i=0; i<clT.size(); i++) {    
    if(visited_var[var(clT[i])])  continue;
#if _print_    
  printf("flip %s%d\n", sign(clT[i])? "-" :"" ,var(clT[i])+1);
#endif     
    visited_var[var(clT[i])] = true;
    flip(rm, var(clT[i]));
    singletonSft = singletonHrd = false;
    bool sat_sft = satisfy_clS(rm, var(clT[i]));
    if(singletonSft || sat_sft) {
      if(singletonSft && (visited[uns_sft] == stamp_cand)) goto undoFlip;
      bool sat_hrd = satisfy_clH(rm, var(clT[i]));
      if(singletonSft && sat_hrd) {
#if _print_
       printf("uns_sft : %d\n", uns_sft+1);
#endif       	
	this->process(rm, false);
      }
      else if(sat_sft && singletonHrd) {
	this->process(rm, false);
#if _print_
       printf("uns_hrd : %d\n", uns_hrd+1);
#endif       

      }
    }

  undoFlip:;
#if _print_    
  printf("undo flip %s%d\n", sign(clT[i])? "-" :"" ,var(clT[i])+1);
#endif      
    flip(rm, var(clT[i])); //restore
    visited_var[var(clT[i])] = false;
  }
  return;
}//process


int ModelRotator::ex2in(Lit s) {
  if(freshVar(var(s))) return freshSelector_map[var(s) - formula.nVars()]; 
  assert(orgSelector_map[toInt(s)] != NULL);
  return *(orgSelector_map[toInt(s)]);  
}//ex2in


Lit ModelRotator::clID2sel(int i) {
  return toLit(*orgID2sel_map[i]);
}

void ModelRotator::setup(RotateModel& rm)
{
  uns_sft = uns_hrd = 0;
  singletonSft = true;
  singletonHrd = false;
  stamp_cand++;

  for(int i=0; i<rm.core.size(); i++) {
    int j = ex2in(rm.core[i]); 
    cand[j] = stamp_cand;
    actived[j] = stamp_core;
  }
  
  uns_sft = ex2in(rm.tc);
  assert(cand[uns_sft] == stamp_cand); 
}//setRotateModel


void ModelRotator::updateSatCore(vec<Lit>& core) {
  stamp_core++;
  for(int i=0; i<core.size(); i++) actived[ex2in(core[i])] = stamp_core;
}

void ModelRotator::flip(RotateModel& rm, const Var x) {
  assert(rm.modelValue(x) != l_Undef);
  rm.model[x] = (rm.model[x] == l_True)? l_False : l_True;
}//flip


bool ModelRotator::satisfied(RotateModel& rm, const vector<Lit>& cl) {
  for(size_t i=0; i<cl.size(); i++) if(rm.modelValue(cl[i]) == l_True) return true;
  return false;
}//satisfied


bool ModelRotator::satisfy_clS(RotateModel& rm, const Var x) {
  int unsat_cpt = 0;
  Lit p =  (rm.modelValue(x) == l_True)? mkLit(x, true) : mkLit(x, false);
  for(size_t i = 0; i<s_occurs[toInt(p)].size(); i++) {
    int occ = s_occurs[toInt(p)][i];
    if(actived[occ] != stamp_core) continue;
    const vector<Lit>& c = formula.getSoft(occ);
    if(!satisfied(rm, c)) {unsat_cpt++; uns_sft = occ; singletonSft = true;}
    if(unsat_cpt > 1) {singletonSft = false; return false;}
  }
  
  return !unsat_cpt;
}//satisfySft


bool ModelRotator::satisfy_clH(RotateModel& rm, Var x)
{
  int cpt = 0;
  Lit p =  (rm.modelValue(x) == l_True)? mkLit(x, true) : mkLit(x, false);
  for(unsigned int i=0; i<h_occurs[toInt(p)].size(); i++) {
    int occ = h_occurs[toInt(p)][i];
    if(!satisfied(rm, formula.getHard(occ))) {cpt++; uns_hrd = occ; singletonHrd = true;}
    if(cpt>1) {singletonHrd = false; return false;}
  }
  
  return !cpt;
}//satisfyHrd
