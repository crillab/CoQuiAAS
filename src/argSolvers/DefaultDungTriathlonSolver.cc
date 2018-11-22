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
	auto gr = groundedExtensions();
	solver->computeAllMss();
	auto st = stableExtensions();
	auto pr = preferredExtensions();
	this->answer = this->formatter.formatD3(gr, st, pr);
}


std::string DefaultDungTriathlonSolver::preferredExtensions() {
	std::vector<std::vector<int> > allMss = solver->getAllMss();
	return this->formatter.formatEveryExtension(allMss);
}

std::string DefaultDungTriathlonSolver::groundedExtensions() {
	ExtensionUtils extUtils(attacks);
	std::vector<int> propagated = extUtils.groundedExtension();
	std::vector<std::vector<int>> vec;
	vec.push_back(propagated);
	return this->formatter.formatEveryExtension(vec);
}


std::string DefaultDungTriathlonSolver::stableExtensions() {
	std::vector<std::vector<int>> stExts;
	std::vector<std::vector<int> > allMss = solver->getAllMss();
	int nMss = (signed) allMss.size();
	ExtensionUtils extUtils(attacks);
	for(int i=0; i<nMss; ++i) {
		if(extUtils.isMaxRange(allMss[i])) stExts.push_back(allMss[i]);
	}
	return this->formatter.formatEveryExtension(stExts);
}


void DefaultDungTriathlonSolver::isCredulouslyAccepted() {
	computeAllExtensions();
}


void DefaultDungTriathlonSolver::isSkepticallyAccepted() {
	computeAllExtensions();
}


DefaultDungTriathlonSolver::~DefaultDungTriathlonSolver() {}

