/*
 * GraphBasedGroundedSemanticsSolver.cc
 *
 *  Created on: 11 janv. 2017
 *      Author: lonca
 */

#include "GraphBasedGroundedSemanticsSolver.h"


using namespace CoQuiAAS;


GraphBasedGroundedSemanticsSolver::GraphBasedGroundedSemanticsSolver(Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter):
	SemanticsProblemSolver(attacks, varMap, taskType, formatter), formatter(formatter) {}


void GraphBasedGroundedSemanticsSolver::init() {}


void GraphBasedGroundedSemanticsSolver::computeOneExtension() {
	ExtensionUtils extUtils(attacks);
	std::vector<int> propagated = extUtils.groundedExtension();
	this->formatter.writeSingleExtension(propagated);
}


void GraphBasedGroundedSemanticsSolver::computeAllExtensions() {
	ExtensionUtils extUtils(attacks);
	std::vector<int> propagated = extUtils.groundedExtension();
	this->formatter.writeExtensionListBegin();
	this->formatter.writeExtensionListElmt(propagated, true);
	this->formatter.writeExtensionListEnd();
}


void GraphBasedGroundedSemanticsSolver::isCredulouslyAccepted() {
	ExtensionUtils extUtils(attacks);
	std::vector<int> grExt = extUtils.groundedExtension();
	int var = varMap.getVar(this->acceptanceQueryArgument);
	for(unsigned int i=0; i<grExt.size(); ++i) {
		if(var == grExt[i]) this->formatter.writeArgAcceptance(true);
		return;
	}
	this->formatter.writeArgAcceptance(false);
}


void GraphBasedGroundedSemanticsSolver::isSkepticallyAccepted() {
	isCredulouslyAccepted();
}


GraphBasedGroundedSemanticsSolver::~GraphBasedGroundedSemanticsSolver() {}
