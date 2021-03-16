#include "cmp/enum/destructCoMSSExtract4Enum.h"

using namespace CMP;

DestructCoMSSExtract4Enum::DestructCoMSSExtract4Enum(WCNF &f) : 
CoMSSExtract4Enum(f) 
{
  phantomVar = 0;
}// constructor

void DestructCoMSSExtract4Enum::search(vec<Lit> &mss, vec<Lit> &unkn, vec<Lit> &coMss)
{
  if(verb > 1) {
    printDashed(53); 
    printf("c | %5s | %8s | %8s | %8s | %8s |\n", "Res", "Time" , "#Unknown", "#MSS", "#coMSS");
    printDashed(53);
  }

  vec<Lit> assums, prev_assums;
  unkn.copyTo(assums);
  mss.copyTo(prev_assums);
  
  while(assums.size())
    {
      int n = 1;
      bool assumsIsCoMss = (assums.size() == 1);
      if(optClauseD) {
	assumsIsCoMss = extendMSS(assums, unkn, mss, prev_assums); 
	if(assumsIsCoMss && (assums.size() > 1)) goto updateCoMss;
      }
      assums[0] = ~assums[0];
      if(optClauseN) { n = assums.size(); clauseN(assums);  n = assums.size() - n+1; }

      if((assums.size() > 1) && !slv->solve(assums, prev_assums))
        {
	  exploiteCore(assums, n);
          if(verb>1) {printf("c | %5s | %8.2lf | %8d | %8d | %8d |\n", "UNS", cpuTime(), unkn.size(), mss.size(), coMss.size());}
        }
      else
        {
	  //assums[0] is a transition clause
	  markedSelector[var(assums[0])] = sign(assums[0]) + 1;
          if(optBackbone) backbone(~assums[0], prev_assums);

	updateCoMss:;
	  vec<Lit> moveToMss, moveToCoMss;
	  shrinkUnknown(moveToMss, moveToCoMss, unkn, assumsIsCoMss);
	  for(int i = 0 ; i<moveToMss.size() ; i++) mss.push(moveToMss[i]);
	  for(int i = 0 ; i<moveToCoMss.size() ; i++) coMss.push(moveToCoMss[i]);

	  unkn.copyTo(assums);
          slv->solve(assums, prev_assums);
	  vec<Lit> core; slv->getConflict(core);
          core2assump(core, assums);
	  if(!assums.size()) {for(int i = 0 ; i<unkn.size() ; i++) mss.push(unkn[i]); unkn.clear();}
          if(verb>1) {printf("c | %5s | %8.2lf | %8d | %8d | %8d |\n", "SAT", cpuTime(), unkn.size(), mss.size(), coMss.size());}
        }
    }
  if(verb>1) printDashed(53);
}//search



/**
   exploite the unsat core (conflict)
*/
void DestructCoMSSExtract4Enum::exploiteCore(vec<Lit>& assumps, int nClauseN)
 {
   vec<Lit> neg_alpha;
   if(slv->originalVar(var(assumps[0]))) neg_alpha.push(assumps[0]);
   for(int i=1; i<nClauseN; i++) neg_alpha.push(assumps[i]);

   vec<Lit> core; slv->getConflict(core);
   
   if(! intersect(core, neg_alpha)) {
     core2assump(core, assumps);
   } 
   else {
     vec<Lit> gamma;
     for(int i=nClauseN; i<assumps.size(); i++) gamma.push(assumps[i]);
     if(!intersect(core, gamma)) {
       markedSelector[var(assumps[0])] = sign(~assumps[0]) + 1;  
     }
     else {
       core.push(~assumps[0]);
       slv->addClause(core);
     }
     //remove clauseN lits in assumps
     if(nClauseN == 1) {assumps[0] = assumps.last();}
     else {for(int i = 0; i<assumps.size() - nClauseN; i++) assumps[i] = assumps[i+nClauseN];}
     assumps.shrink(nClauseN);
   }
 }//expCore


/**
   return true if MSS is extended, flase otherwise
 */
bool DestructCoMSSExtract4Enum::extendMSS(vec<Lit> &assumps, vec<Lit>& unkn, vec<Lit>& mss, vec<Lit>& prev_assump)
{
  if(assumps.size() == 1) {
    //markedSelector[var(assumps[0])] = sign(~assumps[0]) + 1;
    return true;
  }
  slv->allowLearnts(false);
  int nLearnts = slv->nLearnts();
  
  vec<Lit> ps;
  assumps.copyTo(ps);
  if(!phantomVar) phantomVar = slv->newVar();
  ps.push(mkLit(phantomVar, true));
  slv->addPhantomClause(ps);

  vec<Lit> assums2; 
  mss.copyTo(assums2);
  assums2.push(mkLit(phantomVar, false));

  bool res = slv->solve(assums2);
  assums2.clear();
  if(res) {
    res = !trimAssumps(assumps, unkn);
    //
    int i, j;
    for(i=j=0; i<unkn.size(); i++) {
      if(markedSelector[var(unkn[i])] == sign(unkn[i])+1) {mss.push(unkn[i]); prev_assump.push(unkn[i]);}
      else unkn[j++] = unkn[i];
    }
    unkn.shrink(i-j);
  }
  else slv->removeFromTrail(phantomVar);

  slv->removePhantomClauses();
  while(slv->nLearnts() > nLearnts) {slv->popLearnt();
    //    slv->deleteLearnt(slv->getIth_learnts(slv->nLearnts() - 1).ref());
  }
  slv->allowLearnts(true);

  if(!res) {for(int i=0; i<assumps.size(); i++) markedSelector[var(assumps[i])] = sign(~assumps[i]) + 1;}
  return !res;
}//clauseD
