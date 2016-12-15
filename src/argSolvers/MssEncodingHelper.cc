/*
 * MssEncodingHelper.cc
 *
 *  Created on: 22 août 2016
 *      Author: lonca
 */

#include "MssEncodingHelper.h"


using namespace CoQuiAAS;


MssEncodingHelper::MssEncodingHelper(MssSolver &solver, Attacks& attacks, VarMap& varMap) : SatEncodingHelper(solver, attacks, varMap), solver(solver) {}


void MssEncodingHelper::setMaxExtensionNeeded() {
	std::vector<int, std::allocator<int> >* vars = varMap.intVars();
	for(std::vector<int>::iterator itVars = vars->begin() ; itVars != vars->end(); ++itVars) { // iterate over vars representing arguments
		std::vector<int> softClause;
		softClause.push_back(*itVars);
		solver.addSoftClause(softClause);
	}
}


void MssEncodingHelper::setMaxRangeNeeded(int disjVarsStartId) {
	std::vector<int, std::allocator<int> >* vars = varMap.intVars();
	for(std::vector<int>::iterator itVars = vars->begin() ; itVars != vars->end(); ++itVars) { // iterate over vars representing arguments
		std::vector<int> softClause;
		softClause.push_back(*itVars);
		softClause.push_back(*itVars+(disjVarsStartId-1));
		solver.addSoftClause(softClause);
	}
}


MssEncodingHelper::~MssEncodingHelper() {}

