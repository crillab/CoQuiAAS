#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>

#include "core/SolverTypes.h"
#include "core/SolverRL.h"

using namespace std;
using namespace Minisat;

/**
   Constructeur
 */
SolverRL::SolverRL(int nbVar, int lAssumption)
{    
  numatom = nbVar;
  limitAssumption = lAssumption;
  initialized = false;
  
  changed = new int[numatom + 1];  
  breakcount = new int[numatom + 1];
  makecount = new int[numatom + 1];
  isAssign = new unsigned int[numatom + 1];
  atom = new unsigned int[numatom + 1];
  isLitFree = new unsigned int[numatom + 1];

  for(int i = 0 ; i <= nbVar ; i++)
    {
      isAssign[i] = 0; 
      tmpZero.push(0);
    }
  for(int i = 0 ; i <= (2*nbVar) ; i++) occurence.push();
  frequenceAlea = 8;

  addToRecup = weightClause = watch1 = watch2 = numtruelit = falseClause = wherefalse = NULL;
  tabu_length = 7;
  numflip = 0;  

  flippedLit.growTo(nbVar + 1);
}// SolverRL


/**
   Initialization of variable in relation with the set of clauses
 */
void SolverRL::initWithClauses(vec<CRef> &clauses, ClauseAllocator &ca)
{
  currentSizeDB = clauses.size();
  watch1 = (int *) realloc(watch1, sizeof(int) * clauses.size());
  watch2 = (int *) realloc(watch2, sizeof(int) * clauses.size());
  numtruelit = (int *) realloc(numtruelit, sizeof(int) * clauses.size());
  falseClause = (int *) realloc(falseClause, sizeof(int) * clauses.size());
  wherefalse = (int *) realloc(wherefalse, sizeof(int) * clauses.size());
  weightClause = (int *) realloc(weightClause, sizeof(int) * clauses.size());
  addToRecup = (int *) realloc(addToRecup, sizeof(int) * clauses.size());
  
  for(int i = 0 ; i <= (2*numatom) ; i++) occurence[i].clear();
  for(int i = 0 ; i < clauses.size() ; i++)
    {            
      weightClause[i] = 1;
      Clause &cl = ca[clauses[i]];
      for(int j = 0 ; j < cl.size() ; j++) occurence[toInt(cl[j])].push(i);
    }
}// initWithClauses


/**
   Initialization of complete interpretation
*/
void SolverRL::initInterpretation()
{
  for(int i = 0 ; i <= numatom ; i++)
    {
      if(isAssign[i]) atom[i] = isAssign[i] & 1;
      else atom[i] = rand() & 1;
    }
}/* initInterpretation */


/**
 * Initialization of different structure to RL
 */
void SolverRL::initVariableRL(vec<CRef> &clauses, ClauseAllocator &ca)
{
  numfalse = 0;  
  recupScore = false;
  totalAssignFalse = false;

  for(int i = 0 ; i <= numatom ; i++)
    {
      isLitFree[i] = 0;
      changed[i] = -1;
      breakcount[i] = makecount[i] = 0;      
    }
  
  /* Initialize breakcount and makecount in the following: */
  for(int i = 0 ; i < clauses.size() ; i++)
    {
      Clause &cl = ca[clauses[i]];
      Lit thetruelit = lit_Undef;
      Lit truelit1 = lit_Undef;
      Lit truelit2 = lit_Undef;
      int nbAssign = 0;

      addToRecup[i] = cl.toTransfer();
      numtruelit[i] = 0;

      for(int j = 0 ; j < cl.size() ; j++)
	{
          if(isAssign[var(cl[j])]) nbAssign++;
	  if(WASS_TRUE(cl[j], atom[var(cl[j])]))
	    {
	      numtruelit[i]++;
	      thetruelit = cl[j];
	      if (truelit1 == lit_Undef) truelit1 = cl[j];
	      else if ((truelit1 != lit_Undef) && (truelit2 == lit_Undef)) truelit2 = cl[j];
	    }
	}
     
      if(!(numtruelit[i]))
	{ 
	  assert(!cl.toTransfer());
	  wherefalse[i] = numfalse;
	  falseClause[numfalse++] = i;
	  for(int j = 0 ; j < cl.size() ; j++) makecount[var(cl[j])] += weightClause[i];
	}
      else if(numtruelit[i] == 1)
	{
	  breakcount[var(truelit1)] += weightClause[i];
	  watch1[i] = var(truelit1);
	  if(cl.toTransfer()) isLitFree[var(truelit1)]++;
	}
      else /* if (numtruelit[i] == 2) */
	{          
	  watch1[i] = var(truelit1);
	  watch2[i] = var(truelit2);          
	}
    }

  maxSat = maxSatTmp = numfalse;  
  numnullflip = 0;
}// initVariableRL


/**
 * Initialyze : breakcount, makecount, ....
 */
void SolverRL::init(vec<CRef> &clauses, ClauseAllocator &ca)
{  
  initVariableRL(clauses, ca);  
  maxSat = maxSatTmp = numfalse;  
  numnullflip = 0;  
}/* init */ 


/**
   New flipping function based on SAT2004 submission work
   Assign toflip in atom.
*/
void SolverRL::flipatom(Lit toflip, vec<CRef> &clauses, ClauseAllocator &ca)
{     
  int numocc;
  int *occptr;
  int index;
  
  numflip++;
  if((toflip == lit_Undef) || WASS_TRUE(toflip, atom[var(toflip)])){numnullflip++; return;}
    
  assert(!isAssign[var(toflip)]);
  atom[var(toflip)] = (1 ^ atom[var(toflip)]) & 1;  

  numocc = occurence[toInt(~toflip)].size();
  occptr = occurence[toInt(~toflip)];
  changed[var(toflip)] = numflip;
  
  for(int i = 0 ; i < numocc ;i++)
    {
      index = *(occptr++);
      
      assert(index < clauses.size());      
      Clause &cl = ca[clauses[index]];
            
      if (--(numtruelit[index]) == 0)
	{
	  wherefalse[index] = numfalse;
	  falseClause[numfalse++] = index;
	  breakcount[var(toflip)] -= weightClause[index];
	  for (int j = 0 ; j < cl.size() ; j++) makecount[var(cl[j])] += weightClause[index];
	}
      else if(numtruelit[index] == 1)
	{
	  if(watch1[index] == var(toflip)) watch1[index] = watch2[index];
	  breakcount[watch1[index]] += weightClause[index];
	}
      else /* numtruelit[cli] >= 2 */
	{           
	  if(watch1[index] == var(toflip))
	    {
	      /* find a true literal other than watch1[cli] and watch2[cli] */
	      for(int j = 0 ; j < cl.size() ; j++)
		{
		  if(WASS_TRUE(cl[j], atom[var(cl[j])]) && (watch1[index] != var(cl[j])) && (watch2[index] != var(cl[j])))
		    {
		      watch1[index] = var(cl[j]);
		      break;
		    }
		}
	    }
	  else if(watch2[index] == var(toflip))
	    {
	      /* find a true literal other than watch1[cli] and watch2[cli] */
	      for(int j = 0 ; j < cl.size() ; j++)
		{
		  if(WASS_TRUE(cl[j], atom[var(cl[j])]) && (watch1[index] != var(cl[j])) && (watch2[index] != var(cl[j])))
		    {
		      watch2[index] = var(cl[j]);
		      break;
		    }
		}
	    }
	}
    }  
  
  numocc = occurence[toInt(toflip)].size();
  occptr = occurence[toInt(toflip)];

  for(int i = 0 ; i < numocc; i++)
    {    
      index = *(occptr++);
      Clause &cl = ca[clauses[index]];

      if (++(numtruelit[index]) == 1)
	{
	  numfalse--;
	  falseClause[wherefalse[index]] = falseClause[numfalse];
          wherefalse[falseClause[numfalse]] = wherefalse[index];

	  breakcount[var(toflip)] += weightClause[index];
	  for (int j = 0; j < cl.size(); j++) makecount[var(cl[j])] -= weightClause[index];
	  watch1[index] = var(toflip);
	}
      else if(numtruelit[index] ==  2)
	{
	  watch2[index] = var(toflip);
	  breakcount[watch1[index]] -= weightClause[index];
	}
    } 
}/* flipatom */


/**
   Main wsat
 */
lbool SolverRL::wsat(int numtry, vec<CRef> &clauses, ClauseAllocator &ca)
{
  init(clauses, ca);
  if(!numfalse) return l_True;
      
  while(numfalse && numtry)
    {                 
      bornMaxFlips = super(numtry) * 100000;      
 
      for(numflip = 0 ; numflip < bornMaxFlips && numfalse ; numflip++)
        { 
          flipatom(rnovelty(clauses, ca), clauses, ca);
          update_statistics_end_flip();
        }

      if(numfalse)
        {           
          showInter();
          init(clauses, ca);
          nbAlea = numflip = 0;
          numtry--;
        }
    }

  if(!numfalse){ termineProg(SAT,(char *)  "ASSIGNMENTS FOUND");  return l_True;}
  
  numflip = 0;
  termineProg(OUT, (char *) "TIME_OUT");
  return l_Undef;
}/* wsat */


/**
   Rnovelty heuristic
 */
Lit SolverRL::rnoveltyFree(vec<CRef> &clauses, ClauseAllocator &ca, vec<bool> &cbs)
{
  int diff = 0, birthdate;
  Lit youngest = lit_Undef,  best = lit_Undef, second_best = lit_Undef;
  int youngest_birthdate, best_diff, second_best_diff;
  int tmp = 0;

  assert(numfalse);
  Clause &cl = ca[clauses[falseClause[random() % numfalse]]];

  youngest_birthdate = -2;
  best_diff = -BIG;
  second_best_diff = -BIG;

  for(int i = 0 ; i < cl.size() ; i++)
    {
      Lit l = cl[i];
      if(!cbs[var(l)] || var(l) >= limitAssumption) continue; else tmp++;
      
      diff = makecount[var(l)] - breakcount[var(l)];
      birthdate = changed[var(l)];

      if(birthdate > youngest_birthdate)
	{
	  youngest_birthdate = birthdate;
	  youngest = l;
	} 

      if(diff > best_diff || (diff == best_diff && changed[var(l)] < changed[var(best)])) 
	{
	  /* found new best, demote best to 2nd best */
	  second_best = best;
	  second_best_diff = best_diff;
	  best = l;
	  best_diff = diff;
	}
      else if(diff>second_best_diff || (diff== second_best_diff && changed[var(l)] < changed[var(second_best)]))
	{
	  /* found new second best */
	  second_best = l;
	  second_best_diff = diff;
	}
    }

  if(!tmp) return lit_Undef;
  if(tmp == 1) return best;
  if(best != youngest) return best;  
  if((best_diff - second_best_diff) == 1) return second_best;
  if(!(random() & 7)) return second_best;
  return best;
}/* rnovelty */


/**
   The metaheuristic tabu
 */
Lit SolverRL::tabu(vec<CRef> &clauses, ClauseAllocator &ca)
{  
  int numbest;		/* how many are tied for best */
  int bestvalue;		/* best value so far */
  int noisypick;

  Clause &cl = ca[clauses[falseClause[random() % numfalse]]];
  Lit best[cl.size()];	/* best possibility so far */
  vec<Lit> safety;
  int numbreak[cl.size()];  
   
  for(int i = 0 ; i < cl.size() ; i++) numbreak[i] = breakcount[var(cl[i])];    

  numbest = 0;
  bestvalue = BIG;

  noisypick = random() & 1;
  for (int i = 0; i < cl.size() ; i++)
    {
      if(isAssign[var(cl[i])]) continue; 
      safety.push(cl[i]);

      if(numbreak[i] == 0) 
        {
          if (bestvalue > 0){ bestvalue = 0; numbest = 0;}
          best[numbest++] = cl[i];
        }
      else if (tabu_length < numflip - changed[var(cl[i])]) 
        {
          if(noisypick && bestvalue > 0) best[numbest++] = cl[i]; 
          else 
            {
              if (numbreak[i] < bestvalue)
                {
                  bestvalue = numbreak[i];
                  numbest = 0;
                }
              if (numbreak[i] == bestvalue)  best[numbest++] = cl[i]; 
            }
        }
    }
 
  assert(safety.size());
  int rdmE = random() % safety.size();
  if (numbest == 0) return safety[rdmE];
  if (numbest == 1) return best[0];
  return best[random() % numbest];
}/* tabu */


/**
   The metaheuristic tabu
 */
Lit SolverRL::best(vec<CRef> &clauses, ClauseAllocator &ca)
{  
  int numbreak;
  vec<Lit> safety;
  register int numbest;
  register int bestvalue;

  Clause &cl = ca[clauses[falseClause[random() % numfalse]]];
  Lit best[cl.size()];

  numbest = 0;
  bestvalue = BIG;
  
  for (int i = 0; i< cl.size() ; i++)
    {      
      if(isAssign[var(cl[i])]) continue; 
      safety.push(cl[i]);

      numbreak = breakcount[var(cl[i])];
      if (numbreak <= bestvalue)
        {
          if(numbreak<bestvalue) numbest=0;
          bestvalue = numbreak;
          best[numbest++] = cl[i];
        }
    }

  assert(safety.size());
  assert(numbest);
  if(bestvalue>0 && (random() & 1))
    {
      int rdm = random() % safety.size();
      return safety[rdm];;
    }
  
  if(numbest == 1) return best[0];
  return best[random() % numbest];
}/* best */


/**
   rnovelty metaheurstic
 */
Lit SolverRL::rnovelty(vec<CRef> &clauses, ClauseAllocator &ca)
{
  int diff, birthdate;
  Lit youngest = lit_Undef,  best = lit_Undef, second_best = lit_Undef;
  int youngest_birthdate, best_diff, second_best_diff;
  int tmp = 0;

  int indexClause = falseClause[random() % numfalse];
  Clause &cl = ca[clauses[indexClause]];
  
  if(!(numflip & ((1 << frequenceAlea) - 1)))
    {      
      nbAlea++;
      int pos = random() % cl.size();
      return cl[pos]; 
    }
  
  youngest_birthdate = -1;
  best_diff = -BIG;
  second_best_diff = -BIG;

  for(int i = 0 ; i < cl.size() ; i++)
    {
      tmp++;
      diff = makecount[var(cl[i])] - breakcount[var(cl[i])];
      birthdate = changed[var(cl[i])];
      if(birthdate > youngest_birthdate)
	{
	  youngest_birthdate = birthdate;
	  youngest = cl[i];
	}
      if (diff > best_diff || (diff == best_diff && changed[var(cl[i])] < changed[var(best)])) 
	{
	  /* found new best, demote best to 2nd best */
	  second_best = best;
	  second_best_diff = best_diff;
	  best = cl[i];
	  best_diff = diff;
	}
      else if(diff>second_best_diff || (diff== second_best_diff && changed[var(cl[i])] < changed[var(second_best)]))
	{
	  /* found new second best */
	  second_best = cl[i];
	  second_best_diff = diff;
	}
    }
 
  assert(tmp);   
  if(best_diff <= 0)
    {      
      if(!addToRecup[indexClause])
        {
          addToRecup[indexClause] = 1;
          posBestInterpretation.push(indexClause);
        }
    }

  if(tmp == 1 || best != youngest) return best;
  if (breakcount[var(best)] <= breakcount[var(second_best)] && (random() & 1)) return best;
  if (makecount[var(best)] <= makecount[var(second_best)] && (random() & 1)) return second_best;
  return best;
}/* rnovelty */



/**
   One run
   @param[in] nflips, number of flips for this run
   @param[in] clauses, clause data base
   @param[in] ca, to find the clauses with the cref
 */
void SolverRL::oneDescent(int nFlips, vec<CRef> &clauses, ClauseAllocator &ca, vec<Lit> &mt, vec<bool> &cbs)
{
  flippedLit.zero(); 
  int nbNotFind = 1;

#if 0
  while(1)
    {
      printf("numfalse = %d\n", numfalse);
      for(int j = 0 ; j<numfalse ; j++)
        showClause(ca[clauses[falseClause[j]]]);
      printf("--------------------------\n");

      printf("make your choice\n");
      int v = 0;
      scanf("%d", &v);
      
      bool s = (v < 0);
      if(v < 0) v = -v;
      v--;
      
      Lit l = mkLit(v, s);
      flipatom(l, clauses, ca);      
    }
#endif

  
  for(int ite = 0 ; ite < nFlips ; ite += nbNotFind)
    {
#if 0       
      printf("%d/numfalse = %d\n", ite, numfalse);
      for(int j = 0 ; j<numfalse ; j++)
        showClause(ca[clauses[falseClause[j]]]);
      printf("--------------------------\n");
#endif

      nbNotFind += numfalse;
      Lit l = rnoveltyFree(clauses, ca, cbs);      
#if 0
      printf("chosen lit = %d\n", readeableLit(l));
#endif
      if(l == lit_Undef) break;        
      
      if(!tmpZero[var(l)]) flippedLit.push_(l);
      tmpZero[var(l)]++;
      
      flipatom(l, clauses, ca);      
      if(numfalse == 1)
        {
          Clause &c = ca[clauses[falseClause[0]]];
          if(!c.selector() || !cbs[c.selector()]) continue;
          
          mt.push(mkLit(c.selector(), false));
          cbs[c.selector()] = false;
          nFlips += 100;
          nbNotFind = 1;          
        }
    }
  
  for(int i = 0 ; i<flippedLit.size() ; i++)
    {
      if(tmpZero[var(flippedLit[i])] & 1) flipatom(~flippedLit[i], clauses, ca);
      tmpZero[var(flippedLit[i])] = 0;
    }
  return;
}// oneDescent



/**
   Perform the recursive model rotation.  WARNING: for the next we
   suppose that the variable wasHere associated to each clauses is set
   to 0.

   @param[in] (clauses, ca), the problem information 
   @param[out] mt, the vector where are saved the transition constraints found
   @param[in] cbs, flag to know if a selector can be selected
*/
void SolverRL::makeRotation(vec<CRef> &clauses, ClauseAllocator &ca, vec<int> &idx, vec<Lit> &coMss,
                            vec< vec<Lit> > &mt, vec<bool> &cbs, vec<int> &szCores, vec<Lit> &cAss)
{ 
  if(currentSizeDB > clauses.size()) // reinit the local search solver
    {
      initWithClauses(clauses, ca);
      init(clauses, ca);
    }  
  
  for(int i = 0 ; i<coMss.size() ; i++) cbs[var(coMss[i])] = false;

  // collect the concerned literals
  vec<Lit> canBeFlipped;
  vec< vec<int> > associatedPos;
  
  for(int i = 0 ; i<idx.size() ; i++)
    {      
      mt.push();     
      if(!szCores[i] && coMss.size() > 1) continue;
   
      Clause &c = ca[clauses[idx[i]]];
      //showClause(c);

      for(int j = 0 ; j<c.size() ; j++)
        {
          Var v = var(c[j]);
          if(!cbs[v]) continue;            
          
          if(!tmpZero[v])
            {                  
              canBeFlipped.push(c[j]);
              associatedPos.push();
              tmpZero[v] = canBeFlipped.size();
            }
          associatedPos[tmpZero[v] - 1].push(i);
        }
    }

  for(int i = 0 ; i<canBeFlipped.size() ; i++) tmpZero[var(canBeFlipped[i])] = 0; // reinit tmpZero for oneDescent
  for(int i = 0 ; i<canBeFlipped.size() ; i++)
    {      
      vec<Lit> t;  
      cbs[var(canBeFlipped[i])] = false;
      int nbRotation = 50;
      makeRotation_acc(canBeFlipped[i], clauses, ca, t, cbs, nbRotation, cAss);
      cbs[var(canBeFlipped[i])] = true;

      vec<int> &posIdx = associatedPos[i];
      for(int j = 0 ; j<posIdx.size() ; j++) 
        {
          vec<Lit> &rtFound = mt[posIdx[j]];
          for(int k = 0 ; k<rtFound.size() ; k++) cbs[var(rtFound[k])] = true;
          for(int k = 0 ; k<t.size() ; k++) if(!cbs[var(t[k])]) rtFound.push(t[k]); else cbs[var(t[k])] = false;
        }      
      for(int j = 0 ; j<t.size() ; j++) cbs[var(t[j])] = true;
    }
  
  // reinit cbs  
  for(int i = 0 ; i<idx.size() ; i++) cbs[var(coMss[i])] = true;
}// makeRotation


/**
 */
int SolverRL::flipTransition(vec<CRef> &clauses, ClauseAllocator &ca, Lit l, vec<Lit> &cAss)
{
  int numocc;
  int *occptr;
  int index;
  int indexFalse = -1;

  numflip++;
  numocc = occurence[toInt(~l)].size();
  occptr = occurence[toInt(~l)];
  
  for(int i = 0 ; i < numocc ;i++)
    {
      index = *(occptr++);
      
      assert(index < clauses.size());      
      Clause &cl = ca[clauses[index]];
      
      bool isSAT = false;
      for(int j = 0 ; j<cl.size() && !isSAT ; j++) isSAT = cl[j] == cAss[var(cl[j])];
      
      if(!isSAT)
        {
          if(indexFalse != -1) return -1;
          indexFalse = index;
        }
    }

  return indexFalse;
}// flipTransition


/**
   Accumulator for the makeRotation function.
   @param[in] (clauses, ca), the problem information 
   @param[out] mt, the vector where are saved the transition constraints found
   @param[in] cbs, flag to know if a selector can be selected
*/
void SolverRL::makeRotation_acc(Lit l, vec<CRef> &clauses, ClauseAllocator &ca, vec<Lit> &mt,
                                vec<bool> &cbs, int &cNumFlip, vec<Lit> &cAss)
{
  assert(~l == cAss[var(l)]);

  // first at all we search the falsified clause (if more than one then return)
  cNumFlip--;  
  if(cNumFlip <= 0) return;
  
  cAss[var(l)] = l;  
  int pos = flipTransition(clauses, ca, l, cAss);
  if(pos != -1)
    {
      Lit selector = lit_Undef;
      Clause &c = ca[clauses[pos]];  

      if(c.wasHere()) return;
      c.wasHere(1);
  
      for(int i = 0 ; i<c.size() ; i++)
        {
          Var v = var(c[i]);
      
          if(v >= limitAssumption) selector = c[i];
          else if(cbs[v])
            {          
              cbs[v] = false;
              makeRotation_acc(c[i], clauses, ca, mt, cbs, cNumFlip, cAss);
              cbs[v] = true;
            }
        } 
  
      if(selector != lit_Undef && cbs[var(selector)]) 
        {
          cNumFlip += 25;
          cbs[var(selector)] = false;
          mt.push(~selector);
        }
      c.wasHere(0);
    }
  
  cAss[var(l)] = ~l;
}// makeRotation




/////////////////////////////////////////////////////////////
//////////////////////DEBUG PART/////////////////////////////
/////////////////////////////////////////////////////////////

/**
   This function allows to verified if the local search's structure
   are correctly initialized.
*/
void SolverRL::debugTest(vec<CRef> &clauses, ClauseAllocator &ca)
 {   
   static int cpt = 0;
   printf("debug start %d\n", cpt++);
   int mk[numatom + 1], bk[numatom + 1], fal = 0;
   int numT;
   int tmp[2];

   for(int i = 0 ; i <= numatom ; i++)
     {
       mk[i] = bk[i] = 0;
       if(isAssign[i] != NOT_ASS) assert((isAssign[i] & 1) == atom[i]);
       assert(isAssign[i] == NOT_ASS || isAssign[i] == ASS_TRUE || isAssign[i] == ASS_FALSE);
     }
   
   for(int i = 0 ; i<clauses.size() ; i++)
     {
       Clause &cl = ca[clauses[i]];
       
       numT = 0;
       for(int j = 0 ; j < cl.size() ; j++)
         {
           if(sign(cl[j]) != atom[var(cl[j])])
             {
               if(numT < 2) tmp[numT] = var(cl[j]);
               numT++;
             }
           
           /* verified occurence */           
           int k;
           for(k = 0 ; (k < occurence[toInt(cl[j])].size()) && (occurence[toInt(cl[j])][k] != i) ; k++);
           assert(k < occurence[toInt(cl[j])].size());
         }
       
       assert(numtruelit[i] == numT);
       
       if(numT == 0)
         {
	   for(int j = 0 ; j < cl.size() ; j++) 
             {
               mk[var(cl[j])] += weightClause[i];
             }
	   fal++;
         } else if(numT == 1)
         {
           assert(watch1[i] == tmp[0]);
           for(int j = 0 ; j < cl.size() ; j++)
             if(WASS_TRUE(cl[j], atom[var(cl[j])]))
               {
                 bk[var(cl[j])] += weightClause[i];
                 break;
               }
         } else if(numT == 2)
         {
           assert(tmp[0] != tmp[1]);
           assert(watch1[i] == tmp[0] || watch1[i] == tmp[1]);
           assert(watch2[i] == tmp[0] || watch2[i] == tmp[1]);
         }      
     }   
   
   assert(fal == numfalse);
   
   for(int i = 0 ; i < numatom ; i++) 
     {      
       assert(mk[i] == makecount[i]);
       assert(bk[i] == breakcount[i]);
     }
   
   for(int i = 0 ; i<numfalse ; i++) assert(wherefalse[falseClause[i]] == i);
   printf("debugTest end\n");
 }// debugTest
