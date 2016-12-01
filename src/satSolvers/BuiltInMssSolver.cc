/*
 * BuiltInMssSolver.cc
 *
 *  Created on: 21 juil. 2016
 *      Author: lonca
 */


#include "BuiltInMssSolver.h"


using namespace CoQuiAAS;


BuiltInMssSolver::BuiltInMssSolver() : BuiltInSatSolver() {
	this->nSoftCstrs = 0;
}


void BuiltInMssSolver::addSoftClause(std::vector<int> &clause) {
	Minisat::vec<Minisat::Lit> minisatCl;
	intClauseToBuiltInClause(clause, minisatCl);
	solver.addClause_(minisatCl, false);
	++this->nSoftCstrs;
}


void BuiltInMssSolver::computeMss() {
	std::vector<int> assumps;
	computeMss(assumps);
}


void BuiltInMssSolver::computeMss(std::vector<int> &assumps) {
	solver.setSoftInstance(true);
	solver.bigRestart();
	solver.optSaveMCS = true;
	solver.verbosity = 0;
	solver.extractCoMSS();
	vector<int> mss = extractMssFromCoMss(solver.computedMCS[0], this->nSoftCstrs);
	this->mss.push_back(mss);
}


void BuiltInMssSolver::computeAllMss() {
	std::vector<int> assumps;
	computeAllMss(assumps);
}


void BuiltInMssSolver::computeAllMss(std::vector<int> &assumps) {
	solver.setSoftInstance(true);
	solver.bigRestart();
	solver.optSaveMCS = true;
	solver.verbosity = 0;
	solver.enumAllCoMssBlocked();
	for(int i = 0 ; i < solver.computedMCS.size() ; i++){
		vector<int> mss = extractMssFromCoMss(solver.computedMCS[i], this->nSoftCstrs);
		this->mss.push_back(mss);
	}
	solver.foundUnsat = false;
}


bool BuiltInMssSolver::hasAMss() {
	return this->mss.size() > 0;
}


std::vector<int>& BuiltInMssSolver::getMss() {
	return this->mss[0];
}


std::vector<std::vector<int> >& BuiltInMssSolver::getAllMss() {
	return this->mss;
}


BuiltInMssSolver::~BuiltInMssSolver() {}

void BuiltInMssSolver::addVariables(int nVars) {
	BuiltInSatSolver::addVariables(nVars);
}


bool BuiltInMssSolver::addClause(std::vector<int> &clause) {
	return BuiltInSatSolver::addClause(clause);
}


std::vector<int>& BuiltInMssSolver::propagatedAtDecisionLvlZero() {
	solver.setSoftInstance(false);
	return BuiltInSatSolver::propagatedAtDecisionLvlZero();
}


bool BuiltInMssSolver::isPropagatedAtDecisionLvlZero(int lit) {
	solver.setSoftInstance(false);
	return BuiltInSatSolver::isPropagatedAtDecisionLvlZero(lit);
}


void BuiltInMssSolver::computeModel() {
	solver.bigRestart();
	solver.setSoftInstance(false);
	solver.useAsCompleteSolver();
	BuiltInSatSolver::computeModel();
}


void BuiltInMssSolver::computeModel(std::vector<int> &assumps) {
	solver.bigRestart();
	solver.setSoftInstance(false);
	solver.useAsCompleteSolver();
	BuiltInSatSolver::computeModel(assumps);
}


void BuiltInMssSolver::computeAllModels() {
	solver.verbosity = 0;
	//
	solver.bigRestart();
	solver.setSoftInstance(false);
	solver.optSaveMCS = false;
	solver.useAsCompleteSolver();
	BuiltInSatSolver::computeAllModels();
}


void BuiltInMssSolver::computeAllModels(std::vector<int> &assumps) {
	solver.bigRestart();
	solver.setSoftInstance(false);
	solver.useAsCompleteSolver();
	BuiltInSatSolver::computeAllModels(assumps);
}


bool BuiltInMssSolver::hasAModel() {
	solver.setSoftInstance(false);
	return BuiltInSatSolver::hasAModel();
}


std::vector<bool>& BuiltInMssSolver::getModel() {
	solver.setSoftInstance(false);
	return BuiltInSatSolver::getModel();
}


std::vector<std::vector<bool> >& BuiltInMssSolver::getModels() {
	solver.setSoftInstance(false);
	return BuiltInSatSolver::getModels();
}

