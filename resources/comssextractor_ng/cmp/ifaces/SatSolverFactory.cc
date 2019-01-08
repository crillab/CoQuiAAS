#include "cmp/ifaces/SatSolverFactory.h"
#include "minisat/MiniSatSolver.h"
#include "glucose/GlucoseSolver.h"

using namespace CMP;
/*
SatSolver& SatSolverFactory::instance() {
  if(solver != NULL) return *solver;

  if(config = MINISAT) solver = new (SatSolver*) MiniSatSolver();
  else if (config = GLUCOSE) solver = new (SatSolver*) GlucoseSolver();
  else {printf("error solver factory!!\n"); exit(1);}

  return *solver;
}//instance
  
*/
