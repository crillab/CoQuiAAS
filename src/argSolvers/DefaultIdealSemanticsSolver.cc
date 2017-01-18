/*
 * DefaultIdealSemanticsSolver.cc
 *
 *  Created on: 6 déc. 2016
 *      Author: lonca
 */


#include "DefaultIdealSemanticsSolver.h"


using namespace CoQuiAAS;


DefaultIdealSemanticsSolver::DefaultIdealSemanticsSolver(std::shared_ptr<MssSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType)  : SemanticsProblemSolver(attacks, varMap, taskType), solver(solver) {}


void DefaultIdealSemanticsSolver::init() {
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


void DefaultIdealSemanticsSolver::computeOneExtension() {
	solver->computeAllMss();
	std::vector<std::vector<int> > allMss = solver->getAllMss();
	std::vector<bool> argAllowed(varMap.nVars(), true);
	int nMss = (signed) allMss.size();
	for(int i=0; i<nMss; ++i) {
		std::vector<int> mss = allMss[i];
		std::vector<bool> argInMss(varMap.nVars(), false);
		int mssSize = (int) mss.size();
		for(int j=0; j<mssSize; ++j) {
			argInMss[mss[j]-1] = true;
		}
		for(int j=0; j<varMap.nVars(); ++j) {
			argAllowed[j] = argAllowed[j]&argInMss[j];
		}
	}
	std::vector<int> assumps;
	for(int i=0; i<varMap.nVars(); ++i) {
		if(!argAllowed[i]) {
			assumps.push_back(-i-1);
		}
	}
	solver->computeMss(assumps);
	std::vector<int> mss = solver->getMss();
	this->answer = modelToString(mss);
}


void DefaultIdealSemanticsSolver::computeAllExtensions() {
	computeOneExtension();
	this->answer = "["+this->answer+"]";
}


void DefaultIdealSemanticsSolver::isCredulouslyAccepted() {
	solver->computeMss();
	int arg = varMap.getVar(this->acceptanceQueryArgument);
	std::vector<int> mss = solver->getMss();
	for(unsigned int j=0; j<mss.size(); ++j) {
		if(mss[j] == arg) {
			this->answer = "YES";
			return;
		}
	}
	this->answer = "NO";
}


void DefaultIdealSemanticsSolver::isSkepticallyAccepted() {
	return isCredulouslyAccepted();
}


DefaultIdealSemanticsSolver::~DefaultIdealSemanticsSolver() {}


