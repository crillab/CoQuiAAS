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
		this->answer = this->formatter.formatNoExt();
		return;
	}
	std::vector<bool> model = solver->getModel();
	this->answer = this->formatter.formatSingleExtension(model);
}


void DefaultStableSemanticsSolver::computeAllExtensions() {
	solver->computeAllModels();
	this->answer = this->formatter.formatEveryExtension(this->solver->getModels());
}


void DefaultStableSemanticsSolver::isCredulouslyAccepted() {
	std::vector<int> assumps;
	assumps.push_back(varMap.getVar(this->acceptanceQueryArgument));
	solver->computeModel(assumps);
	this->answer = this->formatter.formatArgAcceptance(solver->hasAModel());
}


void DefaultStableSemanticsSolver::isSkepticallyAccepted() {
	std::vector<int> assumps;
	assumps.push_back(-varMap.getVar(this->acceptanceQueryArgument));
	solver->computeModel(assumps);
	this->answer = this->formatter.formatArgAcceptance(!solver->hasAModel());
}


DefaultStableSemanticsSolver::~DefaultStableSemanticsSolver() {}

