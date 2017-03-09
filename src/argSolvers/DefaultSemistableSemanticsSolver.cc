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
	std::vector<std::vector<bool> > models = computeAllSstExtensions();
	this->answer = "[" + modelToString(models[0]);
	for(unsigned int i=1; i<models.size(); ++i) {
		this->answer = this->answer + "," + modelToString(models[i]);
	}
	this->answer = this->answer + "]";
}


std::vector<std::vector<bool>> DefaultSemistableSemanticsSolver::computeAllSstExtensions() {
	solver->computeAllMss();
	auto solverMsses = solver->getAllMss();
	std::vector<std::vector<int>> msses;
	for(int i=0; i<(signed)solverMsses.size(); ++i) {
		msses.push_back(std::vector<int>(solverMsses[i]));
	}
	auto solverModels = solver->getModels();
	std::vector<std::vector<bool>> oldModels;
	for(int i=0; i<(signed)solverModels.size(); ++i) {
		oldModels.push_back(std::vector<bool>(solverModels[i]));
	}
	solver->resetAllMss();
	solver->resetModels();
	int nVars = varMap.nVars();
	std::vector<int> selectors;
	for(int i=0; i<nVars; ++i) {
		std::vector<int> cl;
		cl.push_back(i+1);
		cl.push_back(i+1+nVars);
		selectors.push_back(solver->addSelectedClause(cl));
	}
	std::vector<std::vector<bool>> extModels;
	for(int i=0; i<(signed)msses.size(); ++i) {
//		extModels.push_back(oldModels[i]);
		std::vector<int> assumps;
		for(int j=0; j<nVars; ++j) {
			assumps.push_back(-selectors[j]);
		}
		for(int j=0; j<(signed)msses[i].size(); ++j) {
			assumps[msses[i][j]-1] = selectors[msses[i][j]-1];
		}
//		for(int j=0; j<nVars; ++j) {
//			if(!oldModels[i][j]) assumps.push_back(j+1);
//		}
		solver->computeAllModels(assumps);
		std::vector<std::vector<bool>> newModels = solver->getModels();
		for(int j=0; j<(signed)newModels.size(); ++j) {
			extModels.push_back(newModels[j]);
		}
		solver->resetModels();
	}
	for(int i=0; i<(signed)selectors.size(); ++i) {
		std::vector<int> cl;
		cl.push_back(-selectors[i]);
		solver->addClause(cl);
	}
	return extModels;
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


