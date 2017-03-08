/*
 * DefaultDungTriathlonSolver.cc
 *
 *  Created on: 11 janv. 2017
 *      Author: lonca
 */


#include "DefaultDungTriathlonSolver.h"


using namespace CoQuiAAS;


DefaultDungTriathlonSolver::DefaultDungTriathlonSolver(std::shared_ptr<MssSolver> solver, Attacks &attacks, VarMap &varMap)  : SemanticsProblemSolver(attacks, varMap, TASK_ALL_EXTS), solver(solver) {}


void DefaultDungTriathlonSolver::init() {
	MssEncodingHelper helper(solver, attacks, varMap);
	switch(taskType) {
	case TASK_CRED_INF:
		break;
	default:
		helper.setMaxExtensionNeeded();
	}
	int disjId = helper.reserveDisjunctionVars();
	helper.createAttackersDisjunctionVars(disjId);
	helper.createCompleteEncodingConstraints(disjId);
}


void DefaultDungTriathlonSolver::computeOneExtension() {
	computeAllExtensions();
}


void DefaultDungTriathlonSolver::computeAllExtensions() {
	this->answer = "";
	addGroundedExtensions();
	this->answer += ",";
	solver->computeAllMss();
	addStableExtensions();
	this->answer += ",";
	addPreferredExtensions();
}


void DefaultDungTriathlonSolver::addPreferredExtensions() {
	if(!solver->hasAMss()) {
		this->answer += "[]";
		return;
	}
	this->answer += "[";
	std::vector<std::vector<int> > allMss = solver->getAllMss();
	int nMss = (signed) allMss.size();
	for(int i=0; i<nMss-1; ++i) {
		this->answer = this->answer + modelToString(allMss[i]) + ",";
	}
	if(nMss) this->answer = this->answer + modelToString(allMss[allMss.size()-1]);
	this->answer = this->answer + "]";
}

void DefaultDungTriathlonSolver::addGroundedExtensions() {
	ExtensionUtils extUtils(attacks);
	std::vector<int> propagated = extUtils.groundedExtension();
	this->answer += "[";
	int nPropagated = (signed) propagated.size();
	int nArgs = varMap.nVars();
	int nPropagatedArgs = 0;
	for(int i=0; i<nPropagated; ++i) {
		if(propagated[i] <= nArgs) {
			if(nPropagatedArgs > 0) this->answer = this->answer+",";
			++nPropagatedArgs;
			this->answer = this->answer+varMap.getName(propagated[i]);
		}
	}
	this->answer = this->answer + "]";
}


void DefaultDungTriathlonSolver::addStableExtensions() {
	if(!solver->hasAMss()) {
		this->answer += "[]";
		return;
	}
	this->answer += "[";
	std::vector<std::vector<int> > allMss = solver->getAllMss();
	int nMss = (signed) allMss.size();
	ExtensionUtils extUtils(attacks);
	bool first = true;
	for(int i=0; i<nMss; ++i) {
		if(!extUtils.isMaxRange(allMss[i])) continue;
		if(first) first = false; else this->answer += ",";
		this->answer = this->answer + modelToString(allMss[i]);
	}
	this->answer = this->answer + "]";
}


void DefaultDungTriathlonSolver::isCredulouslyAccepted() {
	computeAllExtensions();
}


void DefaultDungTriathlonSolver::isSkepticallyAccepted() {
	computeAllExtensions();
}


DefaultDungTriathlonSolver::~DefaultDungTriathlonSolver() {}

