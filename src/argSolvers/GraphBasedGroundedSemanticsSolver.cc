/*
 * GraphBasedGroundedSemanticsSolver.cc
 *
 *  Created on: 11 janv. 2017
 *      Author: lonca
 */

#include "GraphBasedGroundedSemanticsSolver.h"


using namespace CoQuiAAS;


GraphBasedGroundedSemanticsSolver::GraphBasedGroundedSemanticsSolver(Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter):
	SemanticsProblemSolver(attacks, varMap, taskType, formatter) {}


void GraphBasedGroundedSemanticsSolver::init() {}


void GraphBasedGroundedSemanticsSolver::computeOneExtension() {
	ExtensionUtils extUtils(attacks);
	std::vector<int> propagated = extUtils.groundedExtension();
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


void GraphBasedGroundedSemanticsSolver::computeAllExtensions() {
	computeOneExtension();
	this->answer = "["+this->answer+"]";
}


void GraphBasedGroundedSemanticsSolver::isCredulouslyAccepted() {
	ExtensionUtils extUtils(attacks);
	std::vector<int> grExt = extUtils.groundedExtension();
	int var = varMap.getVar(this->acceptanceQueryArgument);
	for(unsigned int i=0; i<grExt.size(); ++i) {
		if(var == grExt[i]) this->answer = "YES";
		return;
	}
	this->answer = "NO";
}


void GraphBasedGroundedSemanticsSolver::isSkepticallyAccepted() {
	isCredulouslyAccepted();
}


GraphBasedGroundedSemanticsSolver::~GraphBasedGroundedSemanticsSolver() {}
