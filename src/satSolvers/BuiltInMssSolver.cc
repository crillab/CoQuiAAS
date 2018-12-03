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


void BuiltInMssSolver::clearMss() {
	this->mss.clear();
}


bool BuiltInMssSolver::computeMss() {
	std::vector<int> assumps;
	return computeMss(assumps);
}


bool BuiltInMssSolver::computeMss(std::vector<int> &assumps) {
	clearMss();
	solver.setSoftInstance(true);
	solver.bigRestart();
	solver.optSaveMCS = true;
	solver.verbosity = 0;
	bool foundOne = solver.extractCoMSS();
	if(!foundOne) return false;
	vector<int> mss = extractMssFromCoMss(solver.computedMCS[0], this->nSoftCstrs);
	this->mss.push_back(mss);
	return true;
}


void BuiltInMssSolver::computeAllMss(void (*callback)(std::vector<int>&)) {
	std::vector<int> assumps;
	computeAllMss(callback, assumps);
}


void BuiltInMssSolver::computeAllMss(void (*callback)(std::vector<int>&), std::vector<int> &assumps) {
	this->shouldStopMssEnum = false;
	clearMss();
	solver.setSoftInstance(true);
	solver.bigRestart();
	solver.optSaveMCS = true;
	solver.verbosity = 0;
	solver.enumAllCoMssBlocked();
	for(int i = 0 ; i < solver.computedMCS.size() ; i++){
		vector<int> mss = extractMssFromCoMss(solver.computedMCS[i], this->nSoftCstrs);
		callback(mss);
		this->mss.push_back(mss);
	}
	solver.foundUnsat = false;
}


void stopMssEnum() {
	// TODO
}


bool BuiltInMssSolver::hasAMss() {
	return this->mss.size() > 0;
}


std::vector<int>& BuiltInMssSolver::getMss() {
	return this->mss[this->mss.size()-1];
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


int BuiltInMssSolver::addSelectedClause(std::vector<int> &clause) {
	return BuiltInSatSolver::addSelectedClause(clause);
}


std::vector<int>& BuiltInMssSolver::propagatedAtDecisionLvlZero() {
	solver.setSoftInstance(false);
	return BuiltInSatSolver::propagatedAtDecisionLvlZero();
}


bool BuiltInMssSolver::isPropagatedAtDecisionLvlZero(int lit) {
	solver.setSoftInstance(false);
	return BuiltInSatSolver::isPropagatedAtDecisionLvlZero(lit);
}


bool BuiltInMssSolver::computeModel() {
	solver.bigRestart();
	solver.setSoftInstance(false);
	solver.useAsCompleteSolver();
	return BuiltInSatSolver::computeModel();
}


bool BuiltInMssSolver::computeModel(std::vector<int> &assumps) {
	solver.bigRestart();
	solver.setSoftInstance(false);
	solver.useAsCompleteSolver();
	return BuiltInSatSolver::computeModel(assumps);
}


void BuiltInMssSolver::computeAllModels(void (*callback)(std::vector<bool>& model)) {
	solver.verbosity = 0;
	//
	solver.bigRestart();
	solver.setSoftInstance(false);
	solver.optSaveMCS = false;
	solver.useAsCompleteSolver();
	BuiltInSatSolver::computeAllModels(callback);
}


void BuiltInMssSolver::computeAllModels(void (*callback)(std::vector<bool>& model), std::vector<int> &assumps) {
	solver.bigRestart();
	solver.setSoftInstance(false);
	solver.useAsCompleteSolver();
	BuiltInSatSolver::computeAllModels(callback, assumps);
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

