#ifndef coMSSEnum_h
#define coMSSEnum_h

#include <functional>

#include "cmp/enum/coMSSExtract4Enum.h"

namespace CMP {

enum e_CMP_Alg {CONSTRUCTIVE, DESTRUCTIVE };

struct Config_CoMSSEnum
{
Config_CoMSSEnum() : clD(false), clN(false), bb(false), mr(false), cache(false), appx(0), verb(0), nb(0) {}
  e_CMP_Alg alg;
  bool clD;
  bool clN;
  bool bb;
  bool mr;
  bool cache;
  int appx;
  int verb;
  int nb;
};

class CoMSSEnum : public MetaSolver
{
 protected:
  // CoMSSExtract4Enum* exttor;
  int nbCoMSS;
  int verb;
  int max;
  
  bool blockDown(vec<Lit>& coMss);

 public:
  CoMSSEnum(WCNF &f, Config_CoMSSEnum config);
  virtual ~CoMSSEnum() {}
  virtual void run();
  virtual void run(vec<Lit>& assumps, std::function<void(vec<int>&, vector<lbool>&)> callback);
  CoMSSExtract4Enum* exttor;

  inline void interrupt() {
    this->printStats();
  }
  
  inline void printStats() {
   
    printf("c #TC : %d\n", exttor->nb_tc);
    printf("c #CacheTC : %d\n", exttor->nb_cache_tc);
    printf("c #cache : %d\n", exttor->nb_cache);
    printf("c\n"); printf("c #coMSSes : %d\n", nbCoMSS);
    printf("c #calls to SatSolver : %d\n", exttor->getSatSolver()->nCalls()); 
    printf("c CPU time : %g s\n", cpuTime()); printf("c\n");
    printf("c\n");
  }

  inline void stopEnum() {
    this->mustStopEnum = true;
  }

 private:
  bool mustStopEnum = false;
  
};

}//namespace

#endif
