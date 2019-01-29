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
	this->helper = new SatEncodingHelper(solver, attacks, varMap);
	int disjId = this->helper->reserveDisjunctionVars();
	this->helper->createAttackersDisjunctionVars(disjId);
	this->helper->createCompleteEncodingConstraints(disjId);
}


void DefaultGroundedSemanticsSolver::computeOneExtension() {
	clock_t startTime = clock();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	std::vector<int>& propagated = solver->propagatedAtDecisionLvlZero(dynAssumps);
	this->formatter.writeSingleExtension(propagated);
	logSingleExtTime(startTime);
}


void DefaultGroundedSemanticsSolver::computeAllExtensions() {
	clock_t startTime = clock();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	std::vector<int>& propagated = solver->propagatedAtDecisionLvlZero(dynAssumps);
	this->formatter.writeExtensionListBegin();
	this->formatter.writeExtensionListElmt(propagated, true);
	this->formatter.writeExtensionListEnd();
	logOneExtTime(startTime, 1);
	logAllExtsTime(startTime);
}


void DefaultGroundedSemanticsSolver::isCredulouslyAccepted() {
	clock_t startTime = clock();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	bool isPropagated = solver->isPropagatedAtDecisionLvlZero(varMap.getVar(this->acceptanceQueryArgument), dynAssumps);
	this->formatter.writeArgAcceptance(isPropagated);
	logAcceptanceCheckingTime(startTime);
}


void DefaultGroundedSemanticsSolver::isSkepticallyAccepted() {
	isCredulouslyAccepted();
}


DefaultGroundedSemanticsSolver::~DefaultGroundedSemanticsSolver() {}

