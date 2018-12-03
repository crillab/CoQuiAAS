/*
 * DefaultDungTriathlonSolver.cc
 *
 *  Created on: 11 janv. 2017
 *      Author: lonca
 */


#include "DefaultDungTriathlonSolver.h"


using namespace CoQuiAAS;


DefaultDungTriathlonSolver::DefaultDungTriathlonSolver(std::shared_ptr<MssSolver> solver, Attacks &attacks, VarMap &varMap, SolverOutputFormatter &formatter):
	SemanticsProblemSolver(attacks, varMap, TASK_ALL_EXTS, formatter), solver(solver) {}


void DefaultDungTriathlonSolver::init() {
	MssEncodingHelper helper(solver, attacks, varMap);
	switch(taskType) {
	case TASK_CRED_INF:
		break;
	default:
		helper.setMaxExtensionNeeded();
	}
	int disjId = helper.reserveDisjunctionVars();
	helper.createAttackersDisjunctionVars(disjId);
	helper.createCompleteEncodingConstraints(disjId);
}


void DefaultDungTriathlonSolver::computeOneExtension() {
	computeAllExtensions();
}


void DefaultDungTriathlonSolver::computeAllExtensions() {
	this->formatter.writeD3Begin();
	ExtensionUtils extUtils(attacks);
	std::vector<int> propagated = extUtils.groundedExtension();
	this->formatter.writeD3GrExts(propagated);
	solver->computeAllMss(NULL);
	std::vector<std::vector<int>> stExts;
	std::vector<std::vector<int> > allMss = solver->getAllMss();
	int nMss = (signed) allMss.size();
	for(int i=0; i<nMss; ++i) {
		if(extUtils.isMaxRange(allMss[i])) stExts.push_back(allMss[i]);
	}
	this->formatter.writeD3StExts(stExts);
	this->formatter.writeD3PrExts(allMss);
	this->formatter.writeD3End();
}


void DefaultDungTriathlonSolver::isCredulouslyAccepted() {
	computeAllExtensions();
}


void DefaultDungTriathlonSolver::isSkepticallyAccepted() {
	computeAllExtensions();
}


DefaultDungTriathlonSolver::~DefaultDungTriathlonSolver() {}

