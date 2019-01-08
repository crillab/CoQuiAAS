#ifndef SatSolverFactory_h
#define SatSolverFactory_h

#include "cmp/ifaces/SatSolver.h"
#include "minisat/MiniSatSolver.h"
#include "glucose/GlucoseSolver.h"


namespace CMP {
  enum SAT_SOLVER {MINISAT, GLUCOSE};
  
  class SatSolverFactory {
  public:
  SatSolverFactory(SAT_SOLVER s): config(s), solver(NULL) {}
  virtual ~SatSolverFactory() { if(solver != NULL) {delete solver; solver = NULL;} }
    
  //SatSolver& instance();
inline SatSolver& instance() {
  if(solver != NULL) return *solver;

  if(config == MINISAT) {solver = (SatSolver*) new MiniSatSolver();}
  else if (config == GLUCOSE) {solver = (SatSolver*) new GlucoseSolver();}
  else {printf("error solver factory!!\n"); exit(1);}

  return *solver;
}//instance
  

  
  protected:
    SAT_SOLVER config;
    SatSolver* solver;
  };
}
#endif
