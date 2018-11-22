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
		this->answer = this->formatter.formatNoExt();
		return;
	}
	std::vector<bool> model = solver->getModel();
	this->answer = this->formatter.formatSingleExtension(model);
}


void DefaultCompleteSemanticsSolver::computeAllExtensions() {
	solver->computeAllModels();
	this->answer = this->formatter.formatEveryExtension(solver->getModels());
}


void DefaultCompleteSemanticsSolver::isCredulouslyAccepted() {
	std::vector<int> assumps;
	assumps.push_back(varMap.getVar(this->acceptanceQueryArgument));
	solver->computeModel(assumps);
	this->answer = this->formatter.formatArgAcceptance(solver->hasAModel());
}


void DefaultCompleteSemanticsSolver::isSkepticallyAccepted() {
	std::vector<int> assumps;
	assumps.push_back(-varMap.getVar(this->acceptanceQueryArgument));
	solver->computeModel(assumps);
	this->answer = this->formatter.formatArgAcceptance(!solver->hasAModel());
}


DefaultCompleteSemanticsSolver::~DefaultCompleteSemanticsSolver() {
	// TODO Auto-generated destructor stub
}

