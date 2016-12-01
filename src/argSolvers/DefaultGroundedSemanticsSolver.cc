/*
 * DefaultGroundedSemanticsSolver.cc
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */

#include "DefaultGroundedSemanticsSolver.h"


using namespace CoQuiAAS;


DefaultGroundedSemanticsSolver::DefaultGroundedSemanticsSolver(SatSolver &satSolver, Attacks &attacks, VarMap &varMap, TaskType taskType) : SemanticsProblemSolver(attacks, varMap, taskType), solver(satSolver) {}


void DefaultGroundedSemanticsSolver::init() {
	SatEncodingHelper helper(this->solver, this->attacks, this->varMap);
	int disjId = helper.reserveDisjunctionVars();
	helper.createAttackersDisjunctionVars(disjId);
	helper.createCompleteEncodingConstraints(disjId);
}


void DefaultGroundedSemanticsSolver::computeOneExtension() {
	std::vector<int>& propagated = solver.propagatedAtDecisionLvlZero();
	this->answer = "[";
	int nPropagated = (signed) propagated.size();
	int nArgs = varMap.nVars();
	int nPropagatedArgs = 0;
	for(int i=0; i<nPropagated; ++i) {
		if(propagated[i] <= nArgs) {
			if(nPropagatedArgs > 0) this->answer = this->answer+",";
			++nPropagatedArgs;
			this->answer = this->answer+varMap.getName(propagated[i]);
		}
	}
	this->answer = this->answer + "]";
}


void DefaultGroundedSemanticsSolver::computeAllExtensions() {
	computeOneExtension();
	this->answer = "["+this->answer+"]";
}


void DefaultGroundedSemanticsSolver::isCredulouslyAccepted() {
	bool isPropagated = solver.isPropagatedAtDecisionLvlZero(varMap.getVar(this->acceptanceQueryArgument));
	this->answer = isPropagated ? "YES" : "NO";
}


void DefaultGroundedSemanticsSolver::isSkepticallyAccepted() {
	isCredulouslyAccepted();
}


DefaultGroundedSemanticsSolver::~DefaultGroundedSemanticsSolver() {}

