/*
 * DefaultStableSemanticsSolver.cc
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */


#include "DefaultStableSemanticsSolver.h"


using namespace CoQuiAAS;


DefaultStableSemanticsSolver::DefaultStableSemanticsSolver(std::shared_ptr<SatSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter):
	SemanticsProblemSolver(attacks, varMap, taskType, formatter), solver(solver) {}


void DefaultStableSemanticsSolver::init() {
	SatEncodingHelper helper(solver, attacks, varMap);
	helper.createStableEncodingConstraints();
}


void DefaultStableSemanticsSolver::computeOneExtension() {
	solver->computeModel();
	if(!solver->hasAModel()) {
		this->formatter.writeNoExt();
		return;
	}
	std::vector<bool> model = solver->getModel();
	this->formatter.writeSingleExtension(model);
}


void DefaultStableSemanticsSolver::computeAllExtensions() {
	this->formatter.writeExtensionListBegin();
	bool first = true;
	bool* firstpt = &first;
	solver->computeAllModels([this, firstpt](std::vector<bool>& model){
		this->formatter.writeExtensionListElmt(model, *firstpt);
		*firstpt = false;
	});
	this->formatter.writeExtensionListEnd();
}


void DefaultStableSemanticsSolver::isCredulouslyAccepted() {
	std::vector<int> assumps;
	assumps.push_back(varMap.getVar(this->acceptanceQueryArgument));
	solver->computeModel(assumps);
	this->formatter.writeArgAcceptance(solver->hasAModel());
}


void DefaultStableSemanticsSolver::isSkepticallyAccepted() {
	std::vector<int> assumps;
	assumps.push_back(-varMap.getVar(this->acceptanceQueryArgument));
	solver->computeModel(assumps);
	this->formatter.writeArgAcceptance(!solver->hasAModel());
}


DefaultStableSemanticsSolver::~DefaultStableSemanticsSolver() {}

