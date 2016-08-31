/*
 * DefaultPreferredSemanticsSolver.cpp
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */


#include "DefaultPreferredSemanticsSolver.h"


using namespace CoQuiAAS;


DefaultPreferredSemanticsSolver::DefaultPreferredSemanticsSolver(MssSolver &solver, Attacks &attacks, VarMap &varMap, TaskType taskType)  : SemanticsProblemSolver(attacks, varMap, taskType), solver(solver) {}


void DefaultPreferredSemanticsSolver::init() {
	MssEncodingHelper helper(solver, attacks, varMap);
	switch(taskType) {
	case TASK_CRED_INF:
		break;
	default:
		helper.setMaxExtensionNeeded();
	}
	int nVars = varMap.nVars();
	helper.createAttackersDisjunctionVars(nVars);
	helper.createCompleteEncodingConstraints(nVars);
}


void DefaultPreferredSemanticsSolver::computeOneExtension() {
	solver.computeMss();
	if(!solver.hasAMss()) {
		this->answer = "NO";
	}
	std::vector<int> mss = solver.getMss();
	this->answer = modelToString(mss);
}


void DefaultPreferredSemanticsSolver::computeAllExtensions() {
	solver.computeAllMss();
	if(!solver.hasAMss()) {
		this->answer = "[]";
	}
	this->answer = "[";
	std::vector<std::vector<int> > allMss = solver.getAllMss();
	int nMss = (signed) allMss.size();
	for(int i=0; i<nMss-1; ++i) {
		this->answer = this->answer + modelToString(allMss[i]) + ",";
	}
	if(nMss) this->answer = this->answer + modelToString(allMss[allMss.size()-1]);
	this->answer = this->answer + "]";
}


void DefaultPreferredSemanticsSolver::isCredulouslyAccepted() {
	std::vector<int> assumps;
	assumps.push_back(varMap.getVar(this->acceptanceQueryArgument));
	solver.computeModel(assumps);
	this->answer = solver.hasAModel() ? "YES" : "NO";
}


void DefaultPreferredSemanticsSolver::isSkepticallyAccepted() {
	solver.computeAllMss();
	std::vector<std::vector<int> > allMss = solver.getAllMss();
	int arg = varMap.getVar(this->acceptanceQueryArgument);
	for(unsigned int i=0; i<allMss.size(); ++i) {
		std::vector<int> mss = allMss[i];
		bool found = false;
		for(unsigned int j=0; j<mss.size(); ++j) {
			if(mss[j] == arg) {
				found = true;
				break;
			}
		}
		if(!found) {
			this->answer = "NO";
			return;
		}
	}
	this->answer = "YES";
}


DefaultPreferredSemanticsSolver::~DefaultPreferredSemanticsSolver() {}

