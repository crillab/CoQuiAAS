
#include "cmp/core/destructCoMSSExtract.h"

using namespace CMP;

DestructCoMSSExtract::DestructCoMSSExtract(WCNF &f) : CoMSSExtract(f)
{
  printf("c Destructive CoMss Extractor\n");
  phantomVar = 0;
}// constructor


/**
   Search a coMss of the input formula store in the solver and where
   the soft clauses are given by the set of selectors.

   @param[out] coMssSelectors, the set of selectors which represents the computed coMss
 */
void DestructCoMSSExtract::search(vec<Lit> &mss, vec<Lit> &unknown, vec<Lit> &coMss)
{
  if(verb){
    printDashed(53);
    printf("c | %5s | %8s | %8s | %8s | %8s |\n", "Res", "Time" , "#Unknown", "#MSS", "#coMSS");
    printDashed(53);
  }
  
  for(int i = 0 ; i<mss.size() ; i++) if(slv->value(mss[i]) == l_Undef) slv->uncheckedEnqueue(mss[i]);

  vec<Lit> assums;
  unknown.copyTo(assums);
  unknown.copyTo(coMss);
    
  while(assums.size())
    {
      int n = 1;
      bool assumsIsCoMss = (assums.size() == 1);
      if(optClauseD) {
	assumsIsCoMss = extendMSS(assums, unknown);
	if(assumsIsCoMss && assums.size()>1) goto updateCoMss;
      }
      pickNextTC(assums);
      assums[0] = ~assums[0]; 
      if(optClauseN) {n = assums.size(); clauseN(assums); n = assums.size()-n+1;} 
      
      if((assums.size() > 1) && !slv->solve(assums))
        {
	  exploiteCore(assums, n);
          if(verb) printf("c | %5s | %8.2lf | %8d | %8d | %8d |\n", "UNS", cpuTime(), unknown.size(), mss.size(), coMss.size());
        }
      else
        {
	  if(slv->value(assums[0]) == l_Undef) slv->uncheckedEnqueue(assums[0]); // transition clause            
	  if(optBackbone) backbone(~assums[0]);

	updateCoMss:;
	  shrinkCoMSS(unknown, assumsIsCoMss);
          slv->solve(unknown);
	  vec<Lit> core; slv->getConflict(core);
          core2assump(core, assums);
	  if(!assums.size()) {for(int i = 0 ; i<unknown.size() ; i++) slv->uncheckedEnqueue(unknown[i]);} //add to MSS 
          if(verb) printf("c | %5s | %8.2lf | %8d | %8d | %8d |\n", "SAT", cpuTime(), unknown.size(), mss.size(), coMss.size());
        }
    }  

  int i,j;
  for(i = j = 0 ; i<coMss.size() ; i++) if(slv->value(coMss[i]) == l_False) coMss[j++] = coMss[i];
  coMss.shrink(i-j);

  if(verb) printDashed(53);
}// search


/**
   exploite the unsat core (conflict)
*/
void DestructCoMSSExtract::exploiteCore(vec<Lit>& assumps, int nClauseN)
 {
   assert(nClauseN < assumps.size());

   vec<Lit> neg_alpha, core;

   if(slv->originalVar(var(assumps[0]))) neg_alpha.push(assumps[0]);
   for(int i=1; i<nClauseN; i++) neg_alpha.push(assumps[i]);

   slv->getConflict(core);
   
   if(! intersect(core, neg_alpha)) {
     core2assump(core, assumps);
   } 
   else {
     vec<Lit> gamma;
     for(int i=nClauseN; i<assumps.size(); i++) gamma.push(assumps[i]);
     if(! intersect(core, gamma)) {
       assert(slv->value(assumps[0]) == l_Undef);
       slv->uncheckedEnqueue(~assumps[0]);  
     }
     else {
       vec<Lit> ps;
       core.copyTo(ps);
       ps.push(~assumps[0]);
       slv->addClause(ps); // slv->createNewClause(slv->blocking, ps);
     }
     //remove clauseN lits in assumps
     if(nClauseN == 1) {assumps[0] = assumps.last();}
     else {for(int i = 0; i<assumps.size() - nClauseN; i++) assumps[i] = assumps[i+nClauseN];}
     assumps.shrink(nClauseN);
   }
   
   slv->cancelUntil(0);
 }//expCore


/**
   return true if MSS is extended, flase otherwise
 */
bool DestructCoMSSExtract::extendMSS(vec<Lit> &assumps, const vec<Lit>& unknown)
{
  if(assumps.size() == 1) return true;
  slv->allowLearnts(false);
  int nLearnts = slv->nLearnts();
  
  vec<Lit> ps;
  assumps.copyTo(ps);
  if(!phantomVar) phantomVar = slv->newVar();
  ps.push(mkLit(phantomVar, true));
  slv->addPhantomClause(ps);
  
  bool res = slv->solve(mkLit(phantomVar, false));

  if(res) res = !trimAssumps(assumps, unknown);
  else slv->removeFromTrail(phantomVar);

  slv->removePhantomClauses();
  while(slv->nLearnts() > nLearnts) {slv->popLearnt();
    // slv->removeClause(slv->learnts.last());
    // slv->learnts.pop();
  }
  slv->allowLearnts(true);

  if(!res) {//assums is coMss
    for(int i=0; i<assumps.size(); i++) {
      if(slv->value(assumps[i]) == l_Undef) slv->uncheckedEnqueue(~assumps[i]);
      if(optBackbone) backbone(assumps[i]);
    }
  }
  return !res;
}//clauseD
