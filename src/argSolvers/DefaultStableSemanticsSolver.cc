/*
 * DefaultStableSemanticsSolver.cc
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */


#include "DefaultStableSemanticsSolver.h"


using namespace CoQuiAAS;


DefaultStableSemanticsSolver::DefaultStableSemanticsSolver(std::shared_ptr<SatSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter):
	SemanticsProblemSolver(attacks, varMap, taskType, formatter), solver(solver) {
	this->solver->setBlockingClauseFunction([this](std::vector<bool>& model) -> std::vector<int> {
		std::vector<int> intCl;
		for(int i=0; i<this->varMap.nVars(); ++i) {
			if(model[i]) intCl.push_back(-i-1);
		}
		return intCl;
	});
}


void DefaultStableSemanticsSolver::init() {
	this->helper = new SatEncodingHelper(solver, attacks, varMap);
	this->helper->createStableEncodingConstraints();
}


void DefaultStableSemanticsSolver::computeOneExtension() {
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	solver->computeModel(dynAssumps);
	if(!solver->hasAModel()) {
		this->formatter.writeNoExt();
		return;
	}
	std::vector<bool> model = solver->getModel();
	this->formatter.writeSingleExtension(model);
}


void DefaultStableSemanticsSolver::computeAllExtensions() {
	this->formatter.writeExtensionListBegin();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	bool first = true;
	solver->computeAllModels([this, &first](std::vector<bool>& model){
		this->formatter.writeExtensionListElmt(model, first);
		first = false;
	}, dynAssumps);
	this->formatter.writeExtensionListEnd();
}


void DefaultStableSemanticsSolver::isCredulouslyAccepted() {
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	dynAssumps.push_back(varMap.getVar(this->acceptanceQueryArgument));
	solver->computeModel(dynAssumps);
	this->formatter.writeArgAcceptance(solver->hasAModel());
}


void DefaultStableSemanticsSolver::isSkepticallyAccepted() {
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	dynAssumps.push_back(-varMap.getVar(this->acceptanceQueryArgument));
	solver->computeModel(dynAssumps);
	this->formatter.writeArgAcceptance(!solver->hasAModel());
}


DefaultStableSemanticsSolver::~DefaultStableSemanticsSolver() {}

