#ifndef coMSSExtract_h
#define coMSSExtract_h

#include <iostream>
#include <vector>
#include <map>

#include "cmp/utils/System.h"
#include "cmp/utils/Vec.h"
#include "cmp/utils/Sort.h"
#include "cmp/utils/cmpTypes.h"
#include "cmp/utils/wcnf.h"

#include "cmp/ifaces/MetaSolver.h"
#include "cmp/ifaces/SatSolver.h"
#include "cmp/ifaces/SatSolverFactory.h"

#include "cmp/approx/eCoMSSApprox.h"
#include "cmp/approx/bCoMSSApprox.h"
#include "cmp/approx/eCoMSSApprox4PMS.h"
#include "cmp/approx/bCoMSSApprox4PMS.h"

using namespace std;

namespace CMP {

class CoMSSExtract : public MetaSolver
{
 private:
  vector<int> mapIdxClauseInSolver;
  
 protected:
  WCNF& formula;
  SatSolverFactory sfact;
  SatSolver* slv;
  vec<Lit> selectors;
  vec<int> markedSelector;
  map<int, vector<int> > mapIdxClause;    
  vector<int*> mapOrgID2in;
  vector<int*> orgSelector_map;
  vector<int> freshSelector_map;
  //
  bool optBackbone; //literal backbone
  bool optClauseD; //clause disjunction
  bool optClauseN; //clause negation
  //
  int optApprox;
  
 public:
  int verb;
 protected:
  //
  int conf_lim;
  int local_search;
  

  virtual void search(vec<Lit> &mss, vec<Lit> &unknown, vec<Lit> &coMss) = 0;
  
 public:
  CoMSSExtract(WCNF &f);
  virtual ~CoMSSExtract();
  void addSoftCstr(WCNF& f, vec<Lit>& ps, int i);
  
  void operator () (vec<Lit>& coMss);
  void operator () (vec<Lit>& mss, vec<Lit>& coMss);
  void shrinkUnknown(vec<Lit> &approxMSS, vec<Lit> &approxCoMSS, vec<Lit> &unknown, bool isCoMss);
    
  inline void interrupt(){slv->interrupt();}    
  inline void printStats(){slv->printStats();}
  inline SatSolver* getSatSolver() {return slv;};
   
  inline void configOpt(bool bb=false, bool clD=false, bool clN=false, int app=0, int v=0, int lim=INT32_MAX, int ls=0)
  {
    optBackbone = bb;
    optClauseD = clD;
    optClauseN = clN;
    optApprox=app;
    verb = v;
    //
    conf_lim = lim;
    local_search = ls;
  }//configOpt

  
  inline void printModel(vec<Lit>& coMss)
  {
    printf("c %s\n",(coMss.size())? "UNSATISFIABLE" : "SATISFIABLE");
    if(!coMss.size())  return;
    vec<int> model;
    in2ex(coMss, model);
    sort(model);
    printf("c #coMSS : %d\n",model.size());
    printf("v "); for(int i = 0 ; i<model.size() ; i++) printf("%d ", model[i]);
    printf("0\nc\n");
  }//printModel

 protected:
  
  inline void printDashed(int nb){printf("c "); for(int i = 0 ; i<nb ; i++) printf("-"); printf("\n"); }
  inline bool pmsFormula() {return (formula.nHards() != 0);}

  void backbone(Lit sel);
  int sel2cl(Lit p);
  bool isSelector(Lit p);
 public:
  void in2ex(vec<Lit> &cs, vec<int> &out);
 protected:
  vector<Lit> ex2in(vector<int> idCls);

  //======================DEBUG=======================//
  void check(vec<Lit> &coMSS);    

};

}//namespace

#endif
