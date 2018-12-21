/*
 * BuiltInMssSolverNG.cc
 *
 *  Created on: 21 juil. 2016
 *      Author: lonca
 */


#include "BuiltInMssSolverNG.h"


using namespace CoQuiAAS;


BuiltInMssSolverNG::BuiltInMssSolverNG() : BuiltInSatSolverNG() {
	this->nSoftCstrs = 0;
}


void BuiltInMssSolverNG::addSoftClause(std::vector<int> &clause) {
	CMP::vec<CMP::Lit> cmpCl;
	toCmpClause(clause, cmpCl);
	this->formula.addSoft(cmpCl, this->nSoftCstrs);
	++this->nSoftCstrs;
}


void BuiltInMssSolverNG::clearMss() {
	this->mss.clear();
}


bool BuiltInMssSolverNG::computeMss() {
	std::vector<int> assumps;
	return computeMss(assumps);
}

bool BuiltInMssSolverNG::computeMss(std::vector<int> &assumps) {
	computeSomeMsses(NULL, assumps, 1);
	return true;
}


void BuiltInMssSolverNG::computeAllMss(std::function<void(std::vector<int>&)> callback) {
	std::vector<int> assumps;
	computeAllMss(callback, assumps);
}


void BuiltInMssSolverNG::computeAllMss(std::function<void(std::vector<int>&)> callback, std::vector<int> &assumps) {
	computeSomeMsses(callback, assumps, INT32_MAX);
}


void BuiltInMssSolverNG::computeSomeMsses(std::function<void(std::vector<int>&)> callback, std::vector<int> &assumps, int maxCount) {
	this->shouldStopMssEnum = false;
	clearMss();
	CMP::Config_CoMSSEnum config =  CMP::Config_CoMSSEnum();
	config.alg = CONSTRUCTIVE;
	config.clD = true;
	config.clN = true;
	config.bb = true;
	config.mr = false;
	config.cache = false;
	config.appx = 0;
	config.verb = 0;
	config.nb = maxCount;
	this->mcsEnumerator = new CoMSSEnum(this->formula, config);
	this->mcsEnumerator->run([this, callback] (CMP::vec<CMP::Lit>& mcs) {
		vector<int> mss = extractMssFromCoMss(mcs, this->nSoftCstrs);
		if(callback) callback(mss);
		this->mss.push_back(mss);
	});
}


bool BuiltInMssSolverNG::hasAMss() {
	return this->mss.size() > 0;
}


std::vector<int>& BuiltInMssSolverNG::getMss() {
	return this->mss[this->mss.size()-1];
}


std::vector<std::vector<int> >& BuiltInMssSolverNG::getAllMss() {
	return this->mss;
}


BuiltInMssSolverNG::~BuiltInMssSolverNG() {
	delete this->solver;
}

void BuiltInMssSolverNG::addVariables(int nVars) {
	addVariables(nVars, false);
}

void BuiltInMssSolverNG::addVariables(int nVars, bool auxVar) {
	for(int i=0; i<nVars; ++i) this->formula.newVar();
}


bool BuiltInMssSolverNG::addClause(std::vector<int> &clause) {
	CMP::vec<CMP::Lit> cmpCl;
	toCmpClause(clause, cmpCl);
	this->formula.addHard(cmpCl);
	return true; // TODO
}


int BuiltInMssSolverNG::addSelectedClause(std::vector<int> &clause) {
	return BuiltInSatSolverNG::addSelectedClause(clause);
}


std::vector<int>& BuiltInMssSolverNG::propagatedAtDecisionLvlZero() {
	return BuiltInSatSolverNG::propagatedAtDecisionLvlZero();
}


std::vector<int>& BuiltInMssSolverNG::propagatedAtDecisionLvlZero(std::vector<int> assumps) {
	return BuiltInSatSolverNG::propagatedAtDecisionLvlZero(assumps);
}


bool BuiltInMssSolverNG::isPropagatedAtDecisionLvlZero(int lit) {
	//solver.setSoftInstance(false);
	return BuiltInSatSolverNG::isPropagatedAtDecisionLvlZero(lit);
}

bool BuiltInMssSolverNG::isPropagatedAtDecisionLvlZero(int lit, std::vector<int> assumps) {
	//solver.setSoftInstance(false);
	return BuiltInSatSolverNG::isPropagatedAtDecisionLvlZero(lit, assumps);
}


bool BuiltInMssSolverNG::computeModel() {
	/* solver.bigRestart();
	solver.setSoftInstance(false);
	solver.useAsCompleteSolver(); */
	return BuiltInSatSolverNG::computeModel();
}


bool BuiltInMssSolverNG::computeModel(std::vector<int> &assumps) {
	/* solver.bigRestart();
	solver.setSoftInstance(false);
	solver.useAsCompleteSolver(); */
	return BuiltInSatSolverNG::computeModel(assumps);
}


void BuiltInMssSolverNG::computeAllModels(std::function<void(std::vector<bool>&)> callback) {
	/* solver.verbosity = 0;
	solver.bigRestart();
	solver.setSoftInstance(false);
	solver.optSaveMCS = false;
	solver.useAsCompleteSolver(); */
	BuiltInSatSolverNG::computeAllModels(callback);
}


void BuiltInMssSolverNG::computeAllModels(std::function<void(std::vector<bool>&)> callback, std::vector<int> &assumps) {
	/* solver.bigRestart();
	solver.setSoftInstance(false);
	solver.useAsCompleteSolver(); */
	BuiltInSatSolverNG::computeAllModels(callback, assumps);
}


bool BuiltInMssSolverNG::hasAModel() {
	//solver.setSoftInstance(false);
	return BuiltInSatSolverNG::hasAModel();
}


std::vector<bool>& BuiltInMssSolverNG::getModel() {
	//solver.setSoftInstance(false);
	return BuiltInSatSolverNG::getModel();
}


std::vector<std::vector<bool> >& BuiltInMssSolverNG::getModels() {
	//solver.setSoftInstance(false);
	return BuiltInSatSolverNG::getModels();
}

void BuiltInMssSolverNG::resetAllMss() {
	this->mss.clear();
}

void BuiltInMssSolverNG::resetModels() {
	this->models.clear();
}

void BuiltInMssSolverNG::stopMssEnum(){
	this->shouldStopMssEnum = true;
}

void BuiltInMssSolverNG::toCmpClause(std::vector<int> &clause, CMP::vec<CMP::Lit>& cmpCl) {
	for(std::vector<int>::iterator it = clause.begin(); it != clause.end(); ++it) {
		int lit = *it;
		CMP::Lit minisatLit = lit > 0 ? CMP::mkLit(lit-1) : ~CMP::mkLit(-lit-1);
		cmpCl.push(minisatLit);
	}
}