
#ifndef LocalSearch_h
#define LocalSearch_h


using namespace std;

#include "minisat/core/SolverTypes.h"

using namespace Minisat;

namespace CMP {

  /* Constant for heuristic */
#define NUMTRUELIT 1
#define WHEREFALSE 2
#define WATCH1 3
#define WATCH2 4
#define WEIGHT 5
#define NB_EXTEND 6

#define ASS_TRUE 1
#define ASS_FALSE 2
#define NOT_ASS 0

#define SAT 1
#define UNS 2
#define OUT 3
#define BIG 1<<29

#define WASS_TRUE(l,ass) (((sign(l)) & 1) ^ (ass))
#define WASS_FALSE(l,ass) (!(((sign(l)) & 1) ^ (ass)))

#define NB_META 3

  /**
   * Solveur de recherche locale
   */
  class LocalSearch
  {
    bool initialized;
  public:
    LocalSearch(int nbVar, int limitAssumption);
    void initInterpretation();
    void initVariableRL(vec<CRef> &clauses, ClauseAllocator &ca);
    void init(vec<CRef> &clauses, ClauseAllocator &ca);
    void flipatom(Lit toflip, vec<CRef> &clauses, ClauseAllocator &ca);
    lbool wsat(int numTry, vec<CRef> &clauses, ClauseAllocator &ca);
    void initWithClauses(vec<CRef> &clauses, ClauseAllocator &ca);
    void initWithClauses(vec<CRef> &clauses, ClauseAllocator &ca, vec<int>& weights); //WCNF formula
    inline void setRecupScore(bool v){recupScore = v;}
    
    void oneDescent(int nFlips, vec<CRef> &clauses, ClauseAllocator &ca, vec<Lit> &mt, vec<bool> &cbs);
    
    bool refineAssigns(bool optWSAT, vec<CRef> &clauses, ClauseAllocator &ca, vec<lbool> &assigns);
    void refineSoftPartAssigns(bool optWSAT, vec<CRef> &clauses, ClauseAllocator &ca, vec<lbool> &assigns);
    void init_wsat4PMS(vec<CRef> &clauses, ClauseAllocator &ca);
    Lit bestLitToFlip(vec<CRef> &clauses, ClauseAllocator &ca);
    void flipatom4PMS(Lit toflip, vec<CRef> &clauses, ClauseAllocator &ca);
    lbool wsat_(int numTry, vec<CRef> &clauses, ClauseAllocator &ca);
    lbool wsat4PMS(int numTry, vec<CRef> &clauses, ClauseAllocator &ca);
    
    inline void adjustIsAssign(vec<Lit> &v, int z)
    {
      clearIsAssign();
      for(int i = 0 ; i<z ; i++)
        {
          isAssign[var(v[i])] = sign(v[i]) + 1;        
          atom[var(v[i])] = isAssign[var(v[i])] & 1;
        }
    }

    inline void clearIsAssign(){for(int i = 0 ; i <= numatom ; i++) isAssign[i] = 0;}
    inline int getMaxSAT(){return maxSat;}
    inline int getNumFalse(){return numfalse;}
    inline long long int getNumFlip(){return numflip;}


    /* show clause information */
    inline void showClause(const Clause &c)
    {
      printf("%d:: ", c.index());
      for(int i = 0 ; i<c.size() ; i++) printf("%d ", readeableLit(c[i]));
      printf("0\n");
    }

    /**
       In complet interpretation with vector of literals
     */
    inline void initInterpretationWithVecLit(vec<CRef> &clauses, ClauseAllocator &ca, vec<Lit> &v)
    {
      int cpt = 0;
      for(int i = 0 ; i<v.size() ; i++)
        {
          if(initialized && atom[var(v[i])] == ((unsigned int) 1 - sign(v[i]))) continue;
          if(!initialized) atom[var(v[i])] = 1 - sign(v[i]); 
          else{cpt++; flipatom(v[i], clauses, ca);}
        }

      if(!initialized) init(clauses, ca); 
      initialized = true;
      // printf("the number of modification during the phase %d\n", cpt);
      // if(cpt && cpt < 40) exit(0);
    }
    
    /**
       This function allows to show the intermediate informations
    */
    inline void showInter()
    {
      fprintf(stdout, "| %6d | %7lld | %7d | %7d |\n", numtry + 1, numflip, maxSatTmp, nbAlea);
    }// showInter    


  private:
    int ok;
    bool noBodyFound;

    /************************************/
    /* Main data structures             */
    /************************************/
    int limitAssumption;        /* all literal greater than limitAssumption are assumption */

    int frequenceAlea;
    int numatom;   

    int currentSizeDB; 
    int initMaxFlips;
    int bornMaxFlips;
    int *falseClause;		/* clauses which are false */
    int numfalse;	      	/* number of false clauses */
    //--------------------------------------------------//
    int h_numfalse;
    int s_numfalse;
    //-------------------------------------------------//
    int *wherefalse;		/* where each clause is listed in false */
    int *weightClause;		/* weight assigned at each clause */
    int *addToRecup;
    
    int *watch1;
    int *watch2;

    int *numtruelit;		/* number of true literals in each clause */
    int *changed;    	        /* step at which atom was last flipped */
    unsigned int *atom;		/* value of each atom */
    unsigned int *bestAtom;

    unsigned int *isLitFree;        /* true if any clauses attached is unisatisfied by this lit, false otherwise */

    vec<Lit> flippedLit;            /* vector used to avoid the reallocation */
    vec<int> posBestInterpretation; /* give falsified clauses by the best interpretation */
    vec<int> tmpZero;               /* tabular always assigned to zero */

    vec< vec<int> > occurence;      /* where each literal occurs */
    unsigned int numnullflip;       /* number of times each literal occurs */

    /* RL Control */
    int *breakcount;	        /* number of clauses that become unsat if var if flipped */
    int *makecount; 	        /* number of clauses that become sat if var if flipped */
    //-------------------------------------------------------------------------------------------------//
    int   *h_breakcount;       // number of hard clauses that become unsat
    int   *s_breakcount;       // number of soft clauses that become unsat
    int    *h_makecount;
    int    *s_makecount;
    bool    *markFlipped;
    //------------------------------------------------------------------------------------------------//
    long long int numflip;	/* number of changes so far */  
    int maxSat;
    int maxSatTmp;     
    int nbAlea;
    int numtry;

    bool recupScore;
    bool totalAssignFalse;
    int numerator;
    int denominator;	
    
    unsigned int *isAssign;  

    Lit (*pickcode)();
  
    /* Function used in local search solver */
    void debugTest(vec<CRef> &clauses, ClauseAllocator &ca);
    Lit rnovelty(vec<CRef> &clauses, ClauseAllocator &ca);

    int tabu_length;
    Lit tabu(vec<CRef> &clauses, ClauseAllocator &ca);
    Lit best(vec<CRef> &clauses, ClauseAllocator &ca);
    Lit rnoveltyFree(vec<CRef> &clauses, ClauseAllocator &ca, vec<bool> &cbs);

    /* Function defined inline */
    /* Performs the next flip */
    inline void performNextFlip(vec<CRef> &clauses, ClauseAllocator &ca, int useMeta)
    {
      Lit toFlip = lit_Undef;
        
      switch(useMeta)
        {
        case 0:
          toFlip = tabu(clauses, ca);
          break;
        case 1: 
          toFlip = rnovelty(clauses, ca);
          break;
        default:
          toFlip = best(clauses, ca);
        }
      
      flipatom(toFlip, clauses, ca);
    }// performNextFlip


    inline void showFormula(vec<CRef> &clauses, ClauseAllocator &ca)
    {
      for(int i = 0 ; i<clauses.size() ; i++)
        {
          printf("%d ---> %d: ", ca[clauses[i]].index(), numtruelit[i]);
          showClause(ca[clauses[i]]);
        } 
    }

    inline bool isHardClause(int index) { return (weightClause[index]>1);}

  public:
    inline vec<int>& getPosBestInterpretation(){return posBestInterpretation;}
    void makeRotation(vec<CRef> &clauses, ClauseAllocator &ca, vec<int> &idx, vec<Lit> &coMss,
                      vec< vec<Lit> > &mt, vec<bool> &cbs, vec<int> &szCores, vec<Lit> &cAss);
    void makeRotation_acc(Lit l, vec<CRef> &clauses, ClauseAllocator &ca, vec<Lit> &mt,
                          vec<bool> &cbs, int &cNumFlip, vec<Lit> &cAss);

    int flipTransition(vec<CRef> &clauses, ClauseAllocator &ca, Lit l, vec<Lit> &cAss);
    
    inline void giveSolution()
    {      

      printf("s SATISFIABLE\n");
      printf("v ");
      for(int i = 0 ; i<numatom; i++)
	{
	  if(isAssign[i]) assert(atom[i] == (isAssign[i] & 1));
	  if(atom[i]) printf("%d ", i + 1); else printf("%d ",-i - 1);
	}
      printf("0\n");
      exit(10);
    }

    /**
     * The program is finished
     */
    inline void termineProg(int res, char *comment)
    {
      fprintf(stdout, "c Local Search Solver find: %s\n", comment);
      fprintf(stdout, "c ");
      fprintf(stdout, "c \nc -----------------------------------\n");
      fprintf(stdout, "c numtry = %d\n", numtry);
      fprintf(stdout, "c numflips = %lld\n", numflip);
      fprintf(stdout, "c maxSat = %d\n", maxSat);
      fprintf(stdout, "c \n");
    

      switch(res)
        {
        case SAT :
          {
            printf("s SATISFIABLE\n");

            printf("v ");
            for(int i = 0 ; i<numatom; i++)
              {
                if(isAssign[i]) assert(atom[i] == (isAssign[i] & 1));
                if(atom[i]) printf("%d ", i + 1); else printf("%d ",-i - 1);
              }
            printf("0\n");
            exit(10);
            break;
          }
        case UNS :
          {
            printf("s UNSATISFIABLE\n");
            exit(20);
            break;
          }
        case OUT : 
          {
            printf("s UNKNOWN\n");
          }
        }  
    }// termineProg

    /**
     * Update statistics end flip
     */
    inline void update_statistics_end_flip()
    {      
      if(maxSatTmp >= numfalse)
        {          
          maxSatTmp = numfalse;
          if(maxSatTmp < maxSat) {
	    maxSat = maxSatTmp;
	    // save the best interpretation
	    for(int i = 0; i<=numatom; i++) bestAtom[i] = atom[i];
	  }          
        }
    }/* update_statistics_end_flip */

    /**
     *  Luby
     */
    inline long super(int i)
    {
      long power;
      int k;

      if (i<=0){
        fprintf(stderr, "c bad argument super(%d)\n", i);
        exit(1);
      }
      /* let 2^k be the least power of 2 >= (i+1) */
      k = 1;
      power = 2;
      while (power < (i+1)){
        k += 1;
        power *= 2;
      }
      if (power == (i+1)) return (power/2);
      return (super(i - (power/2) + 1));
    }/* super */       
  };
}
#endif
