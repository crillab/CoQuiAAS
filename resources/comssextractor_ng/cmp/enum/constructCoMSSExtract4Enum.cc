#include "cmp/enum/constructCoMSSExtract4Enum.h"

using namespace CMP;

ConstructCoMSSExtract4Enum::ConstructCoMSSExtract4Enum(WCNF &f, bool mr, bool c) : CoMSSExtract4Enum(f, mr, c)
{stamp=0; cand = vector<int>(slv->nVars(), 0);}// constructor

void ConstructCoMSSExtract4Enum::search(vec<Lit> &mss, vec<Lit> &unknown, vec<Lit> &coMss)
{
  if(!slv->solve(mss)) {printf("unsat\n"); exit(7);}
  if(verb > 1) {
    printDashed(53); 
    printf("c | %5s | %8s | %8s | %8s | %8s |\n", "Res", "Time" , "#Unknown", "#MSS", "#coMSS"); 
    printDashed(53);
  }
  
  bool prev_sat = false;
  vec<Lit> assums, prev_assums, core;
  mss.copyTo(prev_assums);
  for(int i=0; i<coMss.size(); i++) prev_assums.push(~coMss[i]);
  if(rotation) m_rottor->updateSatCore(mss);
  if(caching) cache->reset();
  
  while(unknown.size())
    {
      Lit ls = unknown.last();
      unknown.pop();
      
      if(markedSelector[var(ls)] && (markedSelector[var(ls)] != sign(ls) + 1)) {
	coMss.push(ls);
	if(rotation && prev_sat) addSingleTC(ls);
	continue;
      }

      if(caching) {
	mss.push(ls); bool ret = cache->hasSubSet(mss);	mss.pop();	
	if(ret) {
	  coMss.push(ls);
	  prev_assums.push(~ls);
	  markedSelector[var(ls)] = sign(~ls)+1;
	  if(rotation && prev_sat) {cache->getConflict(core);printLits(mss);printLits(coMss); mappingTC(ls, core);}
	  nb_cache++;
	  continue;
	}
      }
      
      assums.clear();
      assums.push(ls);
      
      if(slv->solve(assums, prev_assums)) {	
	mss.push(ls);
	prev_assums.push(ls);
	markedSelector[var(ls)] = sign(ls) + 1;
	prev_sat = true;
	vec<Lit> moveToMss, moveToCoMss;
	shrinkUnknown(moveToMss, moveToCoMss, unknown, false); 
	for(int i = 0; i<moveToMss.size(); i++) {mss.push(moveToMss[i]); prev_assums.push(moveToMss[i]);}
	for(int i = 0; i<moveToCoMss.size(); i++) coMss.push(moveToCoMss[i]);
	if(moveToCoMss.size() && rotation) {for(int i=0; i<moveToCoMss.size(); i++) addSingleTC(moveToCoMss[i]);}
	if(verb > 1) printf("c | %5s | %8.2lf | %8d | %8d | %8d |\n", "SAT", cpuTime(), unknown.size(), mss.size(), coMss.size());
      }
      else {
	coMss.push(ls);
	prev_assums.push(~ls);
	markedSelector[var(ls)] = sign(~ls) + 1;
	if(optBackbone && !caching) backbone(ls, prev_assums);
	if(caching) {slv->getConflict(core); cache->store(core);}
	if(rotation && prev_sat) {slv->getConflict(core); mappingTC(ls, core);}
	if (verb > 1) printf("c | %5s | %8.2lf | %8d | %8d | %8d |\n", "UNS", cpuTime(), unknown.size(), mss.size(), coMss.size());
      }
    }
  if(verb > 1) printDashed(53);
 
}//search


void ConstructCoMSSExtract4Enum::mappingTC(Lit clT, vec<Lit>& confl)
{
  //  printf("core ");printLits(confl);
  stamp++;
  int i,j;
  for(i=j=0; i<confl.size(); i++) {
    if(((prime_imp[var(confl[i])] != stamp_imp) && 
	!tcMap.core_level[var(confl[i])] && isSelector(~confl[i])) || (toInt(~clT) == toInt(confl[i])) ) { 
      confl[j++] = ~confl[i];
      cand[var(confl[i])] = stamp;
    }
  }
  confl.shrink(i-j);
  assert(confl.size());
  //  printLits(confl);
  
  vector<Lit> crits;
  if(confl.size() == 1) {crits.push_back(clT);} //confl[0]
  else if(caching && cache->getCrits() && false) {
    
    crits.push_back(clT);
    vector<Lit> *ccrits = cache->getCrits();
     for(i=0; i<(int)ccrits->size(); i++) {
       if((cand[var((*ccrits)[i])] == stamp) && (var((*ccrits)[i]) != var(clT))) crits.push_back((*ccrits)[i]);
     }
     if(crits.size() > 1) nb_cache_tc += crits.size();
  }
  else {
    vector<lbool>& model = slv->getModel();
    RotateModel rm = RotateModel(model, confl, clT);
    m_rottor->process(rm);
    crits = ex2in(rm.get_crits());
  }
  assert(crits.size());
  //printf("crits : ");for(i=0; i<(int)crits.size(); i++) {printLit(crits[i]); printf(" ");}printf("\n");

  tcMap.tc_heap.push_back(crits);
  tcMap.n_tc.push_back(tcMap.tc_heap.back().size());
  for(int i=0; i<confl.size(); i++) {
    if(!tcMap.core_level[var(confl[i])]) tcMap.core_level[var(confl[i])] = tcMap.tc_heap.size(); 
  }

  if(caching) cache->store(crits);

  if(verb) printf("#tr = %d\n", (int)tcMap.tc_heap.back().size());
  if(tcMap.tc_heap.back().size() > 1) nb_tc += tcMap.tc_heap.back().size();
  
}//mappingTC
