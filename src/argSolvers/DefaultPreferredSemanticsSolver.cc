/*
 * DefaultPreferredSemanticsSolver.cpp
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */


#include "DefaultPreferredSemanticsSolver.h"


using namespace CoQuiAAS;


DefaultPreferredSemanticsSolver::DefaultPreferredSemanticsSolver(std::shared_ptr<MssSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter):
	SemanticsProblemSolver(attacks, varMap, taskType, formatter), solver(solver) {}


void DefaultPreferredSemanticsSolver::init() {
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


void DefaultPreferredSemanticsSolver::computeOneExtension() {
	solver->computeMss();
	if(!solver->hasAMss()) {
		this->formatter.writeNoExt();
		return;
	}
	std::vector<int> mss = solver->getMss();
	this->formatter.writeSingleExtension(mss);
}


void DefaultPreferredSemanticsSolver::computeAllExtensions() {
	this->formatter.writeExtensionListBegin();
	bool first = true;
	bool* firstpt = &first;
	solver->computeAllMss([this, firstpt](std::vector<int>& model){
		this->formatter.writeExtensionListElmt(model, *firstpt);
		*firstpt = false;
	});
	this->formatter.writeExtensionListEnd();
}


void DefaultPreferredSemanticsSolver::isCredulouslyAccepted() {
	std::vector<int> assumps;
	assumps.push_back(varMap.getVar(this->acceptanceQueryArgument));
	solver->computeModel(assumps);
	this->formatter.writeArgAcceptance(solver->hasAModel());
}


void DefaultPreferredSemanticsSolver::isSkepticallyAccepted() {
	solver->computeAllMss(NULL); // TODO
	std::vector<std::vector<int> > allMss = solver->getAllMss();
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
			this->formatter.writeArgAcceptance(false);
			return;
		}
	}
	this->formatter.writeArgAcceptance(true);
}


DefaultPreferredSemanticsSolver::~DefaultPreferredSemanticsSolver() {}

