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


int MssEncodingHelper::setMaxRangeNeeded(int disjVarsStartId) {
	std::vector<int, std::allocator<int> >* vars = varMap.intVars();
	int selectorsId = reserveVars(vars->size());
	for(std::vector<int>::iterator itVars = vars->begin() ; itVars != vars->end(); ++itVars) { // iterate over vars representing arguments
		std::vector<int> softClause;
		softClause.push_back(*itVars);
		softClause.push_back(*itVars+(disjVarsStartId-1));
		softClause.push_back(-*itVars-(selectorsId-1));
		solver.addSoftClause(softClause);
	}
	return selectorsId;
}

std::vector<int> MssEncodingHelper::getMaxRangeSelectorsInMss(int selectorsId, std::vector<int>& mss) {
	std::vector<int> selectors;
	for(int i=0; i< (signed) mss.size(); ++i) {
		selectors.push_back(mss[i] + selectorsId - 2);
	}
	return selectors;
}


MssEncodingHelper::~MssEncodingHelper() {}

