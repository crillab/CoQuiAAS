#include "cmp/core/constructCoMSSExtract.h"

using namespace CMP;
	
ConstructCoMSSExtract::ConstructCoMSSExtract(WCNF &f) : CoMSSExtract(f)
{
}// constructor


/**
   Search a coMss of the input formula store in the solver and where
   the soft clauses are given by the set of selectors.

   @param[out] mss, the computed mss
   @param[out] coMss, the computed coMss
   @param[in] unknown, the current unknown selector
 */
void ConstructCoMSSExtract::search(vec<Lit> &mss, vec<Lit> &unknown, vec<Lit> &coMss)
{
  // printf("c Constructive CoMss Extractor\n");  
  if(verb){
    printDashed(53);
    printf("c | %5s | %8s | %8s | %8s | %8s |\n", "Res", "Time" , "#Unknown", "#MSS", "#coMSS");
    printDashed(53);
  }
 
  vec<Lit> assums;
  for(int i = 0; i<mss.size(); i++) if(slv->value(mss[i]) == l_Undef) slv->uncheckedEnqueue(mss[i]);
  coMss.clear();
  
  while(unknown.size())
    {
      Lit ls = unknown.last();
      unknown.pop();
      
      if(markedSelector[var(ls)] && markedSelector[var(ls)] != sign(ls) + 1)
	{
	  coMss.push(ls);
	  continue;
	}
      
      assums.push(ls);
      if(slv->solve(assums))
        {
          mss.push(ls); 
	  if(slv->value(mss.last()) == l_Undef) slv->uncheckedEnqueue(ls);
          markedSelector[var(ls)] = sign(ls) + 1;
          
          vec<Lit> moveToMss, moveToCoMss;
          shrinkUnknown(moveToMss, moveToCoMss, unknown, false);
          for(int i = 0 ; i<moveToMss.size() ; i++) {
	    assums.push(moveToMss[i]);
	    mss.push(moveToMss[i]);
	    if(slv->value(mss.last()) == l_Undef) slv->uncheckedEnqueue(mss.last());
	  }
	  for(int i = 0 ; i<moveToCoMss.size() ; i++){coMss.push(moveToCoMss[i]);}
                      
          if(verb) printf("c | %5s | %8.2lf | %8d | %8d | %8d |\n", "SAT", cpuTime(), unknown.size(), mss.size(), coMss.size());
        }
      else
        {
          coMss.push(ls);
          assums[assums.size() - 1] = ~ls;
          markedSelector[var(ls)] = sign(~ls) + 1;
          slv->cancelUntil(assums.size() - 1);
          
          if(optBackbone) backbone(ls);
          if(verb) printf("c | %5s | %8.2lf | %8d | %8d | %8d |\n", "UNS", cpuTime(), unknown.size(), mss.size(), coMss.size());
        }
    }
  if(verb) printDashed(53);
}//search
