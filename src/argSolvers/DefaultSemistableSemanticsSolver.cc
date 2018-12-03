/*
 * DefaultSemistableSemanticsSolver.cc
 *
 *  Created on: 5 déc. 2016
 *      Author: lonca
 */


#include "DefaultSemistableSemanticsSolver.h"


using namespace CoQuiAAS;


DefaultSemistableSemanticsSolver::DefaultSemistableSemanticsSolver(std::shared_ptr<MssSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter):
	SemanticsProblemSolver(attacks, varMap, taskType, formatter), solver(solver) {}


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
		this->formatter.writeNoExt();
		this->answer = "";
		return;
	}
	this->formatter.writeSingleExtension(this->solver->getModel());
	this->answer = "";
}


void DefaultSemistableSemanticsSolver::computeAllExtensions() {
	this->formatter.writeExtensionListBegin();
	bool first = true;
	std::vector<std::vector<bool> > models = computeAllSstExtensions([this, &first](std::vector<bool>& model) {
		this->formatter.writeExtensionListElmt(model, first);
		first = false;
	});
	this->formatter.writeExtensionListEnd();
	this->answer = "";
}


std::vector<std::vector<bool>> DefaultSemistableSemanticsSolver::computeAllSstExtensions(std::function<void(std::vector<bool>&)> callback) {
	this->stopEnum = false;
	std::vector<std::vector<int>> msses;
	std::vector<std::vector<bool>> oldModels;
	std::vector<std::vector<bool>> extModels;
	solver->computeAllMss([this, callback, &msses, &oldModels](std::vector<int>& mss){
		msses.push_back(mss);
		auto mod = std::vector<bool>(solver->getModels()[oldModels.size()]);
		oldModels.push_back(mod);
		if(callback != NULL) callback(mod);
		if(this->stopEnum) solver->stopMssEnum();
	});
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
	for(int i=0; i<(signed)msses.size(); ++i) {
		extModels.push_back(oldModels[i]);
		std::vector<int> assumps;
		for(int j=0; j<nVars; ++j) {
			assumps.push_back(-selectors[j]);
		}
		for(int j=0; j<(signed)msses[i].size(); ++j) {
			assumps[msses[i][j]-1] = selectors[msses[i][j]-1];
		}
		std::vector<int> cl;
		for(int j=0; j<(signed)oldModels[i].size(); ++j) {
			if(!oldModels[i][j]) cl.push_back(j+1);
		}
		auto blockingSel = solver->addSelectedClause(cl);
		assumps.push_back(blockingSel);
		solver->computeAllModels([this,callback,&extModels](std::vector<bool>& model){
			extModels.push_back(model);
			if(callback != NULL) callback(model);
			if(this->stopEnum) solver->stopMssEnum();
		}, assumps);
		std::vector<int> unitCl;
		unitCl.push_back(-blockingSel);
		solver->addClause(unitCl);
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
	bool status = false;
	int arg = varMap.getVar(this->acceptanceQueryArgument);
	std::vector<std::vector<bool> > models = computeAllSstExtensions([this,arg,&status](std::vector<bool>& model){
		if(model[arg-1]) {
			status = true;
			this->stopEnum = true;
		}
	});
	this->formatter.writeArgAcceptance(status);
	this->answer = "";
}


void DefaultSemistableSemanticsSolver::isSkepticallyAccepted() {
	bool status = true;
	int arg = varMap.getVar(this->acceptanceQueryArgument);
	std::vector<std::vector<bool> > models = computeAllSstExtensions([this,arg,&status](std::vector<bool>& model){
		if(!model[arg-1]) {
			status = false;
			this->stopEnum = true;
		}
	});
	this->formatter.writeArgAcceptance(status);
	this->answer = "";
}


DefaultSemistableSemanticsSolver::~DefaultSemistableSemanticsSolver() {}


