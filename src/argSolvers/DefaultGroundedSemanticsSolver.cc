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
	this->formatter.writeSingleExtension(propagated);
	this->answer = "";
}


void DefaultGroundedSemanticsSolver::computeAllExtensions() {
	std::vector<int>& propagated = solver->propagatedAtDecisionLvlZero();
	this->formatter.writeExtensionListBegin();
	this->formatter.writeExtensionListElmt(propagated, true);
	this->formatter.writeExtensionListEnd();
	this->answer = "";
}


void DefaultGroundedSemanticsSolver::isCredulouslyAccepted() {
	bool isPropagated = solver->isPropagatedAtDecisionLvlZero(varMap.getVar(this->acceptanceQueryArgument));
	this->formatter.writeArgAcceptance(isPropagated);
	this->answer = "";
}


void DefaultGroundedSemanticsSolver::isSkepticallyAccepted() {
	isCredulouslyAccepted();
}


DefaultGroundedSemanticsSolver::~DefaultGroundedSemanticsSolver() {}

