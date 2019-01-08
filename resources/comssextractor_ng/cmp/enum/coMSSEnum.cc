#include "cmp/enum/coMSSEnum.h"
#include "cmp/enum/constructCoMSSExtract4Enum.h"
#include "cmp/enum/destructCoMSSExtract4Enum.h"

using namespace CMP;

CoMSSEnum::CoMSSEnum(WCNF &f, Config_CoMSSEnum config) : exttor(NULL), nbCoMSS(0), verb(config.verb), max(config.nb) 
{
  if(config.alg == CONSTRUCTIVE) {
    exttor = new ConstructCoMSSExtract4Enum(f, config.mr, config.cache);
    exttor->configOpt(config.bb, false, false, config.appx, config.verb);
  }
  else {// config.alg == DESTRUCTIVE
    exttor = new DestructCoMSSExtract4Enum(f);
    exttor->configOpt(config.bb, config.clD, config.clN, config.appx, config.verb);
  }

}//constuctor


bool CoMSSEnum::blockDown(vec<Lit>& coMss)
{
  vec<Lit> coMss2;
  for(int i = 0 ; i<coMss.size() ; i++)
    {
      assert((exttor->getSatSolver())->value(coMss[i]) != l_True);
      if((exttor->getSatSolver())->value(coMss[i]) == l_Undef) coMss2.push(coMss[i]);
    }
  
  if(coMss2.size() == 1) (exttor->getSatSolver())->uncheckedEnqueue(coMss2[0]);    
  else if(coMss2.size() > 1) (exttor->getSatSolver())->addBlockingClause(coMss2); //TODO add blocking Clause
  
  return (coMss2.size() == 0);  
}//blockDown


void CoMSSEnum::run() {
  vec<Lit> assumps;
  CoMSSEnum::run(assumps, NULL);
}


void CoMSSEnum::run(CMP::vec<CMP::Lit>& assumps, std::function<void(vec<int>&, vector<lbool>&)> callback)
{
  vec< vec<Lit> > unblocked_coMSSes;
  vec<Lit> coMss, mss, prev_coMss;
  bool emptyBlockClause = false;

  int res = exttor->getSatSolver()->solve(assumps);
  
  while(res && !emptyBlockClause) {
    if(exttor->rotate()) (*exttor)(mss, coMss);
    else (*exttor)(coMss);
    if(!coMss.size() && callback != NULL) {
      vec<int> mcs;
      exttor->in2ex(coMss, mcs);
      callback(mcs, exttor->getSatSolver()->getModel());
    }
    if(!coMss.size()) break;

    if(verb) {printf("c coMSS(%d):\n",nbCoMSS+1); exttor->printModel(coMss);}

    if(callback != NULL) {
      vec<int> mcs;
      exttor->in2ex(coMss, mcs);
      callback(mcs, exttor->getSatSolver()->getModel());
    }

    coMss.copyTo(prev_coMss);
    if(exttor->rotate()) exttor->getUnexplored(mss, coMss);
    else {mss.clear(); coMss.clear();}
    exttor->getSatSolver()->cancelUntil(exttor->getLevel());    
    
    unblocked_coMSSes.push();
    prev_coMss.copyTo(unblocked_coMSSes.last());
    if(!exttor->getLevel()) { //TOCHECK
      while(unblocked_coMSSes.size()) {
    	emptyBlockClause = blockDown(unblocked_coMSSes.last());
    	unblocked_coMSSes.pop();
      }
    }

    if(!coMss.size() && !mss.size()) {
      res = exttor->getSatSolver()->solve2(assumps);
      if(res) exttor->initAssignment(mss);
    }
    ++nbCoMSS;
    if(max && max==nbCoMSS) return;
  }
}//run
