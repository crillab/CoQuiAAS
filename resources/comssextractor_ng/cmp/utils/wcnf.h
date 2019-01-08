#ifndef WCNF_H
#define WCNF_H

#include <vector>
#include "cmp/utils/cmpTypes.h"
#include "cmp/utils/Sort.h"

using namespace std;

namespace CMP {

  struct Sft_Clause
  {
    int ID;
    vector<Lit> lits;
  };
  
  class WCNF {
    vector< vector<Lit> > hard_clauses;
    vector<Sft_Clause> soft_clauses;
    int n_vars;

  public:
    WCNF():n_vars(0) {}
    ~WCNF(){}

    inline void newVar(void) {n_vars++;}
    inline void addHard(vector<Lit> lits) {hard_clauses.push_back(lits);}
    inline void addSoft(vector<Lit> lits, int id) {soft_clauses.push_back((Sft_Clause){id,lits});}

    inline void addHard(vec<Lit>& ps) {
      vector<Lit> lits;
      for(int i=0; i<ps.size(); i++) lits.push_back(ps[i]);
      hard_clauses.push_back(lits);
    }
    
    inline void addSoft(vec<Lit>& ps, int id) {
      vector<Lit> lits;
      sort(ps);
      Lit p;
      int i;
      for(i=0, p = lit_Undef; i<ps.size(); i++) {
	if(ps[i] != p) {p = ps[i]; lits.push_back(ps[i]);} //remove duplicate literals
      }
      soft_clauses.push_back((Sft_Clause){id,lits});
    }

    
    inline int nVars(void) {return n_vars;}
    inline int nSofts(void) {return soft_clauses.size();}
    inline int nHards(void) {return hard_clauses.size();}
    inline int nClauses(void) {return hard_clauses.size()+soft_clauses.size();}

    inline int getSoft_ID(size_t i) {return soft_clauses[i].ID;}
  
    inline const vector<Lit>& getSoft(size_t i) {return soft_clauses[i].lits;}
    inline const vector<Lit>& getHard(size_t i) {return hard_clauses[i];}
    
    inline void getSoft(int i, vec<Lit>& ps) {
      ps.clear(); for(size_t j=0; j<soft_clauses[i].lits.size(); j++) ps.push(soft_clauses[i].lits[j]);
    }
    
    inline void getHard(int i, vec<Lit>& ps) {
      ps.clear(); for(size_t j=0; j<hard_clauses[i].size(); j++) ps.push(hard_clauses[i][j]);
    }

    inline void output() {
      printf("\np wcnf %d %d %d\n",nVars(), nHards()+nSofts(), nSofts()+1);
      for(int i=0; i<nHards(); i++){
	printf("%d ",nSofts()+1);
	for(size_t j=0; j<getHard(i).size(); j++) printf("%c%d ", sign(getHard(i)[j])?'-':' ', var(getHard(i)[j])+1);
	printf("0\n");
      }
      for(int i=0; i<nSofts(); i++){
 	printf("1 ");
	for(size_t j=0; j<getSoft(i).size(); j++) printf("%c%d ", sign(getSoft(i)[j])?'-':' ', var(getSoft(i)[j])+1);
	printf("0\n");
      }
      printf("\n");
    }

     inline void printSoft(int i) {
      printf("1 ");
      for(size_t j=0; j<getSoft(i).size(); j++) printf("%c%d ", sign(getSoft(i)[j])?'-':' ', var(getSoft(i)[j])+1);
      printf("0\n");
    }

     inline void printHard(int i) {
       printf("%d ",nSofts()+1);
       for(size_t j=0; j<getHard(i).size(); j++) printf("%c%d ", sign(getHard(i)[j])?'-':' ', var(getHard(i)[j])+1);
       printf("0\n");
     }
	
  };

}//namespace

#endif
