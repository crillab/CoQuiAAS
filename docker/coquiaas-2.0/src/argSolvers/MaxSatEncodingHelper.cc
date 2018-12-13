/*
 * MaxSatEncodingHelper.cc
 *
 *  Created on: 5 déc. 2016
 *      Author: lonca
 */

#include "MaxSatEncodingHelper.h"


using namespace CoQuiAAS;


MaxSatEncodingHelper::MaxSatEncodingHelper(std::shared_ptr<MaxSatSolver> solver, Attacks& attacks, VarMap& varMap) : SatEncodingHelper(std::static_pointer_cast<SatSolver>(solver), attacks, varMap), solver(solver) {}


void MaxSatEncodingHelper::setMaxExtensionNeeded(int disjVarsStartId) {
	std::vector<int, std::allocator<int> >& vars = varMap.intVars();
	for(std::vector<int>::iterator itVars = vars.begin() ; itVars != vars.end(); ++itVars) { // iterate over vars representing arguments
		std::vector<int> softClause;
		softClause.push_back(*itVars);
		solver->addSoftClause(softClause);
	}
}


void MaxSatEncodingHelper::setMaxRangeNeeded(int disjVarsStartId) {
	setMaxExtensionNeeded(disjVarsStartId);
	std::vector<int, std::allocator<int> >& vars = varMap.intVars();
	for(std::vector<int>::iterator itVars = vars.begin() ; itVars != vars.end(); ++itVars) { // iterate over vars representing arguments
		std::vector<int> softClause;
		softClause.push_back(*itVars+(disjVarsStartId-1));
		solver->addSoftClause(softClause);
	}
}


MaxSatEncodingHelper::~MaxSatEncodingHelper() {}
