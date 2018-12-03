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
	SatEncodingHelper helper(solver, attacks, varMap);
	int disjId = helper.reserveDisjunctionVars();
	helper.createAttackersDisjunctionVars(disjId);
	helper.createCompleteEncodingConstraints(disjId);
}


void DefaultCompleteSemanticsSolver::computeOneExtension() {
	solver->computeModel();
	if(!solver->hasAModel()) {
		this->formatter.writeNoExt();
		this->answer = "";
		return;
	}
	std::vector<bool> model = solver->getModel();
	this->formatter.writeSingleExtension(model);
	this->answer = "";
}


void DefaultCompleteSemanticsSolver::computeAllExtensions() {
	this->formatter.writeExtensionListBegin();
	bool first = true;
	bool* firstpt = &first;
	solver->computeAllModels([this, firstpt](std::vector<bool>& model){
		this->formatter.writeExtensionListElmt(model, *firstpt);
		*firstpt = false;
	});
	this->formatter.writeExtensionListEnd();
	this->answer = "";
}


void DefaultCompleteSemanticsSolver::isCredulouslyAccepted() {
	std::vector<int> assumps;
	assumps.push_back(varMap.getVar(this->acceptanceQueryArgument));
	solver->computeModel(assumps);
	this->formatter.writeArgAcceptance(solver->hasAModel());
	this->answer = "";
}


void DefaultCompleteSemanticsSolver::isSkepticallyAccepted() {
	std::vector<int> assumps;
	assumps.push_back(-varMap.getVar(this->acceptanceQueryArgument));
	solver->computeModel(assumps);
	this->formatter.writeArgAcceptance(!solver->hasAModel());
	this->answer = "";
}


DefaultCompleteSemanticsSolver::~DefaultCompleteSemanticsSolver() {}

