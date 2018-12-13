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
	// SatEncodingHelper helper(this->solver, this->attacks, this->varMap);
	this->helper = new SatEncodingHelper(solver, attacks, varMap);
	int disjId = this->helper->reserveDisjunctionVars();
	this->helper->createAttackersDisjunctionVars(disjId);
	this->helper->createCompleteEncodingConstraints(disjId);
}


void DefaultGroundedSemanticsSolver::computeOneExtension() {
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	std::vector<int>& propagated = solver->propagatedAtDecisionLvlZero(dynAssumps);
	this->formatter.writeSingleExtension(propagated);
}


void DefaultGroundedSemanticsSolver::computeAllExtensions() {
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	std::vector<int>& propagated = solver->propagatedAtDecisionLvlZero(dynAssumps);
	this->formatter.writeExtensionListBegin();
	this->formatter.writeExtensionListElmt(propagated, true);
	this->formatter.writeExtensionListEnd();
}


void DefaultGroundedSemanticsSolver::isCredulouslyAccepted() {
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	bool isPropagated = solver->isPropagatedAtDecisionLvlZero(varMap.getVar(this->acceptanceQueryArgument), dynAssumps);
	this->formatter.writeArgAcceptance(isPropagated);
}


void DefaultGroundedSemanticsSolver::isSkepticallyAccepted() {
	isCredulouslyAccepted();
}


DefaultGroundedSemanticsSolver::~DefaultGroundedSemanticsSolver() {}

