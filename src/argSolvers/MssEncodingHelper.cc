/*
 * MssEncodingHelper.cc
 *
 *  Created on: 22 août 2016
 *      Author: lonca
 */

#include "MssEncodingHelper.h"


using namespace CoQuiAAS;


MssEncodingHelper::MssEncodingHelper(std::shared_ptr<MssSolver> solver, Attacks& attacks, VarMap& varMap) : SatEncodingHelper(std::static_pointer_cast<SatSolver>(solver), attacks, varMap), solver(solver) {}


void MssEncodingHelper::setMaxExtensionNeeded() {
	std::vector<std::string> vars = varMap.getNames();
	for(unsigned int i=0; i<this->varMap.nVars(); ++i) {
		std::vector<int> softClause;
		softClause.push_back(this->varMap.intVars()[i]);
		solver->addSoftClause(softClause);
	}
}


void MssEncodingHelper::setMaxRangeNeeded(int disjVarsStartId) {
	std::vector<std::string> vars = varMap.getNames();
	for(std::vector<std::string>::iterator itVars = vars.begin() ; itVars != vars.end(); ++itVars) {
		std::vector<int> softClause;
		softClause.push_back(varMap.getVar(*itVars));
		softClause.push_back(this->disjunctionVars[*itVars]);
		solver->addSoftClause(softClause);
	}
}


MssEncodingHelper::~MssEncodingHelper() {}

