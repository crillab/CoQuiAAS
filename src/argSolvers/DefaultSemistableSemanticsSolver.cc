/*
 * DefaultSemistableSemanticsSolver.cc
 *
 *  Created on: 5 déc. 2016
 *      Author: lonca
 */


#include "DefaultSemistableSemanticsSolver.h"


using namespace CoQuiAAS;


DefaultSemistableSemanticsSolver::DefaultSemistableSemanticsSolver(std::shared_ptr<MssSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType)  : SemanticsProblemSolver(attacks, varMap, taskType), solver(solver) {}


void DefaultSemistableSemanticsSolver::init() {
	MssEncodingHelper helper(solver, attacks, varMap);
	int disjId = helper.reserveDisjunctionVars();
	helper.setMaxRangeNeeded(disjId);
	helper.createAttackersDisjunctionVars(disjId);
	helper.createCompleteEncodingConstraints(disjId);
}


void DefaultSemistableSemanticsSolver::computeOneExtension() {
	solver->computeMss();
	if(!solver->hasAMss()) {
		this->answer = "NO";
	}
	this->answer = modelToString(solver->getModel());
}


void DefaultSemistableSemanticsSolver::computeAllExtensions() {
	computeAllSstExtensions();
	std::vector<std::vector<bool> > models = solver->getModels();
	this->answer = "[" + modelToString(models[0]);
	for(unsigned int i=1; i<models.size(); ++i) {
		this->answer = this->answer + "," + modelToString(models[i]);
	}
	this->answer = this->answer + "]";
}


void DefaultSemistableSemanticsSolver::computeAllSstExtensions() {
	std::vector<int> selectors;
	while(solver->computeMss(selectors)) {
		std::vector<bool> model = solver->getModel();
		std::vector<int> cl;
		for(unsigned int i=0; i< (unsigned int)varMap.nVars(); ++i) {
			if(!model[i]) cl.push_back(i+1);
		}
		selectors.push_back(solver->addSelectedClause(cl));
	}
	for(unsigned int i=0; i<selectors.size(); ++i) {
		std::vector<int> cl;
		cl.push_back(selectors[i]);
		solver->addClause(cl);
	}
}


void DefaultSemistableSemanticsSolver::isCredulouslyAccepted() {
	computeAllSstExtensions();
	std::vector<std::vector<int> > allMss = solver->getAllMss();
	int arg = varMap.getVar(this->acceptanceQueryArgument);
	for(unsigned int i=0; i<allMss.size(); ++i) {
		std::vector<int> mss = allMss[i];
		for(unsigned int j=0; i<mss.size(); ++j) {
			if(mss[j] == arg) {
				this->answer = "YES";
				return;
			}
		}
	}
	this->answer = "NO";
}


void DefaultSemistableSemanticsSolver::isSkepticallyAccepted() {
	computeAllSstExtensions();
	std::vector<std::vector<int> > allMss = solver->getAllMss();
	int arg = varMap.getVar(this->acceptanceQueryArgument);
	for(unsigned int i=0; i<allMss.size(); ++i) {
		std::vector<int> mss = allMss[i];
		bool found = false;
		for(unsigned int j=0; i<mss.size(); ++j) {
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


DefaultSemistableSemanticsSolver::~DefaultSemistableSemanticsSolver() {}


