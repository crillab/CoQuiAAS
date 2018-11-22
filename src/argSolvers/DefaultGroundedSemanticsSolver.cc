/*
 * DefaultGroundedSemanticsSolver.cc
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */

#include "DefaultGroundedSemanticsSolver.h"


using namespace CoQuiAAS;


DefaultGroundedSemanticsSolver::DefaultGroundedSemanticsSolver(std::shared_ptr<SatSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter)
	: SemanticsProblemSolver(attacks, varMap, taskType, formatter), solver(solver) {}


void DefaultGroundedSemanticsSolver::init() {
	SatEncodingHelper helper(this->solver, this->attacks, this->varMap);
	int disjId = helper.reserveDisjunctionVars();
	helper.createAttackersDisjunctionVars(disjId);
	helper.createCompleteEncodingConstraints(disjId);
}


void DefaultGroundedSemanticsSolver::computeOneExtension() {
	std::vector<int>& propagated = solver->propagatedAtDecisionLvlZero();
	this->answer = this->formatter.formatSingleExtension(propagated);
}


void DefaultGroundedSemanticsSolver::computeAllExtensions() {
	std::vector<int>& propagated = solver->propagatedAtDecisionLvlZero();
	std::vector<std::vector<int>> vec;
	vec.push_back(propagated);
	this->answer = this->formatter.formatEveryExtension(vec);
}


void DefaultGroundedSemanticsSolver::isCredulouslyAccepted() {
	bool isPropagated = solver->isPropagatedAtDecisionLvlZero(varMap.getVar(this->acceptanceQueryArgument));
	this->answer = this->formatter.formatArgAcceptance(isPropagated);
}


void DefaultGroundedSemanticsSolver::isSkepticallyAccepted() {
	isCredulouslyAccepted();
}


DefaultGroundedSemanticsSolver::~DefaultGroundedSemanticsSolver() {}

