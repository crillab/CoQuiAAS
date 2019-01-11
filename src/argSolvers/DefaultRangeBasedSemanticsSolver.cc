#include "DefaultRangeBasedSemanticsSolver.h"


using namespace CoQuiAAS;


DefaultRangeBasedSemanticsSolver::DefaultRangeBasedSemanticsSolver(std::shared_ptr<MssSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter):
	SemanticsProblemSolver(attacks, varMap, taskType, formatter), solver(solver) {}


void DefaultRangeBasedSemanticsSolver::computeOneExtension() {
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	solver->computeMss(dynAssumps);
	if(!solver->hasAMss()) {
		this->formatter.writeNoExt();
		return;
	}
	this->formatter.writeSingleExtension(solver->getModel());
}


void DefaultRangeBasedSemanticsSolver::computeAllExtensions() {
	this->formatter.writeExtensionListBegin();
	bool first = true;
	std::vector<std::vector<bool> > models = computeAllExtensions([this, &first](std::vector<bool>& model) {
		this->formatter.writeExtensionListElmt(model, first);
		first = false;
	});
	this->formatter.writeExtensionListEnd();
}


std::vector<std::vector<bool>> DefaultRangeBasedSemanticsSolver::computeAllExtensions(std::function<void(std::vector<bool>&)> callback) {
	this->stopEnum = false;
	std::vector<std::vector<int>> msses;
	std::vector<std::vector<bool>> oldModels;
	std::vector<std::vector<bool>> extModels;
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	solver->computeAllMss([this, callback, &msses, &oldModels](std::vector<int>& mss, std::vector<bool>& model){
		msses.push_back(mss);
		oldModels.push_back(model);
		if(callback != NULL) callback(model);
		if(this->stopEnum) solver->stopMssEnum();
	}, dynAssumps);
	solver->resetAllMss();
	solver->resetModels();
	if(this->stopEnum) return extModels;
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
		for(int j=0; j<this->varMap.nVars(); ++j) {
			if(!oldModels[i][j]) cl.push_back(j+1);
		}
		for(int j=0; j<dynAssumps.size(); ++j) assumps.push_back(dynAssumps[j]);
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
		if(this->stopEnum) break;
	}
	for(int i=0; i<(signed)selectors.size(); ++i) {
		std::vector<int> cl;
		cl.push_back(-selectors[i]);
		solver->addClause(cl);
	}
	return extModels;
}


void DefaultRangeBasedSemanticsSolver::isCredulouslyAccepted() {
	bool status = false;
	int arg = varMap.getVar(this->acceptanceQueryArgument);
	std::vector<std::vector<bool> > models = computeAllExtensions([this,arg,&status](std::vector<bool>& model){
		if(model[arg-1]) {
			status = true;
			this->stopEnum = true;
		}
	});
	this->formatter.writeArgAcceptance(status);
}


void DefaultRangeBasedSemanticsSolver::isSkepticallyAccepted() {
	bool status = true;
	int arg = varMap.getVar(this->acceptanceQueryArgument);
	std::vector<std::vector<bool> > models = computeAllExtensions([this,arg,&status](std::vector<bool>& model){
		if(!model[arg-1]) {
			status = false;
			this->stopEnum = true;
		}
	});
	this->formatter.writeArgAcceptance(status);
}


DefaultRangeBasedSemanticsSolver::~DefaultRangeBasedSemanticsSolver() {}


