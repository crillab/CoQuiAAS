/*
 * MaxSatEncodingHelper.cc
 *
 *  Created on: 5 déc. 2016
 *      Author: lonca
 */

#include "MaxSatEncodingHelper.h"


using namespace CoQuiAAS;


MaxSatEncodingHelper::MaxSatEncodingHelper(MaxSatSolver &solver, Attacks& attacks, VarMap& varMap) : SatEncodingHelper(solver, attacks, varMap), solver(solver) {}


int MaxSatEncodingHelper::setMaxRangeNeeded(int disjVarsStartId) {
	std::vector<int, std::allocator<int> >* vars = varMap.intVars();
	for(std::vector<int>::iterator itVars = vars->begin() ; itVars != vars->end(); ++itVars) { // iterate over vars representing arguments
		std::vector<int> softClause;
		softClause.push_back(*itVars);
		softClause.push_back(*itVars+(disjVarsStartId-1));
		solver.addSoftClause(softClause);
	}
	return 1;
}


MaxSatEncodingHelper::~MaxSatEncodingHelper() {}
