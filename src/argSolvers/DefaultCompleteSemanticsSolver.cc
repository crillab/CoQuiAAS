/*
 * DefaultCompleteSemanticsSolver.cc
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */


#include "DefaultCompleteSemanticsSolver.h"


using namespace CoQuiAAS;


DefaultCompleteSemanticsSolver::DefaultCompleteSemanticsSolver(std::shared_ptr<SatSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter):
	SemanticsProblemSolver(attacks, varMap, taskType, formatter), solver(solver) {}


void DefaultCompleteSemanticsSolver::init() {
	this->helper = new SatEncodingHelper(solver, attacks, varMap);
	int disjId = this->helper->reserveDisjunctionVars();
	this->helper->createAttackersDisjunctionVars(disjId);
	this->helper->createCompleteEncodingConstraints(disjId);
}


void DefaultCompleteSemanticsSolver::computeOneExtension() {
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	solver->computeModel(dynAssumps);
	if(!solver->hasAModel()) {
		this->formatter.writeNoExt();
		return;
	}
	std::vector<bool> model = solver->getModel();
	this->formatter.writeSingleExtension(model);
}


void DefaultCompleteSemanticsSolver::computeAllExtensions() {
	this->formatter.writeExtensionListBegin();
	bool first = true;
	bool* firstpt = &first;
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	solver->computeAllModels([this, firstpt](std::vector<bool>& model){
		this->formatter.writeExtensionListElmt(model, *firstpt);
		*firstpt = false;
	}, dynAssumps);
	this->formatter.writeExtensionListEnd();
}


void DefaultCompleteSemanticsSolver::isCredulouslyAccepted() {
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	dynAssumps.push_back(varMap.getVar(this->acceptanceQueryArgument));
	solver->computeModel(dynAssumps);
	this->formatter.writeArgAcceptance(solver->hasAModel());
}


void DefaultCompleteSemanticsSolver::isSkepticallyAccepted() {
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	dynAssumps.push_back(-varMap.getVar(this->acceptanceQueryArgument));
	solver->computeModel(dynAssumps);
	this->formatter.writeArgAcceptance(!solver->hasAModel());
}


DefaultCompleteSemanticsSolver::~DefaultCompleteSemanticsSolver() {}

