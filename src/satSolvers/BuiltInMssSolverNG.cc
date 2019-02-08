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
	this->newFormula.addSoft(cmpCl, this->nSoftCstrs);
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


void BuiltInMssSolverNG::computeAllMss(std::function<void(std::vector<int>&, std::vector<bool>&)> callback) {
	std::vector<int> assumps;
	computeAllMss(callback, assumps);
}


void BuiltInMssSolverNG::computeAllMss(std::function<void(std::vector<int>&, std::vector<bool>&)> callback, std::vector<int> &assumps) {
	computeSomeMsses(callback, assumps, INT32_MAX);
}


void BuiltInMssSolverNG::buildEnumerator(int maxCount) {
	for(int i=0; i<this->newFormula.nVars(); ++i) this->formula.newVar();
	vec<Lit> ps;
	for(int i=0; i<this->newFormula.nHards(); ++i) {
		this->newFormula.getHard(i, ps);
		this->formula.addHard(ps);
	}
	for(int i=0; i<this->newFormula.nSofts(); ++i) {
		this->newFormula.getSoft(i, ps);
		this->formula.addSoft(ps, this->formula.nSofts());
	}
	this->newFormula = WCNF();
	Logger::getInstance()->debug("building a new instance the coMSS enumerator");
	Logger::getInstance()->debug("formula has %d variables, %d hard clauses and %d soft clauses", this->formula.nVars(), this->formula.nHards(), this->formula.nSofts());
	CMP::Config_CoMSSEnum config = CMP::Config_CoMSSEnum();
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
	Logger::getInstance()->debug("restoring %d learnt clauses", learnts.size());
	for(unsigned int i=0; i<this->learnts.size(); ++i) ((MiniSatSolver*) this->mcsEnumerator->exttor->getSatSolver())->slv->restoreLearnt(learnts[i]);
}


void BuiltInMssSolverNG::computeSomeMsses(std::function<void(std::vector<int>&, std::vector<bool>&)> callback, std::vector<int> &assumps, int maxCount) {
	this->shouldStopMssEnum = false;
	clearMss();
	buildEnumerator(maxCount);
	((MiniSatSolver*) this->mcsEnumerator->exttor->getSatSolver())->slv->phase_saving = 0;
	CMP::vec<CMP::Lit> assumptions;
	for(std::vector<int>::iterator it = assumps.begin(); it != assumps.end(); ++it) {
		int lit = *it;
		CMP::Lit minisatLit = lit > 0 ? CMP::mkLit(lit-1) : ~CMP::mkLit(-lit-1);
		assumptions.push(minisatLit);
	}
	int nVars = this->mcsEnumerator->exttor->getSatSolver()->nVars();
	this->mcsEnumerator->run(assumptions, [this, callback, nVars] (CMP::vec<int>& mcs, std::vector<CMP::lbool> model) {
		CMP::vec<CMP::Lit> litMcs;
		for(int i=0; i<mcs.size(); ++i) litMcs.push(CMP::mkLit(mcs[i], false));
		vector<int> mss = extractMssFromCoMss(litMcs, this->nSoftCstrs);
		vector<bool> mod;
		for(unsigned int i=0; i<model.size(); ++i) mod.push_back(model[i] == l_True);
		if(callback) callback(mss, mod);
		this->mss.push_back(mss);
		this->models.push_back(mod);
	});
	storeLearnts(nVars);
	Logger::getInstance()->debug("solver has %d learnt clauses", ((MiniSatSolver*) this->mcsEnumerator->exttor->getSatSolver())->slv->nLearnts());
}


void BuiltInMssSolverNG::storeLearnts(int nVarsAtCall) {
	if(!shouldStoreLearnts) return;
	Minisat::Solver* slv = ((MiniSatSolver*) this->mcsEnumerator->exttor->getSatSolver())->slv;
	this->learnts.clear();
	for(int i=0; i<slv->nLearnts(); ++i) {
		std::vector<Minisat::Lit> clCp;
		Minisat::Clause& cl = slv->ca[slv->learnts[i]];
		bool defined = true;
		for(int j=0; j<cl.size() && defined; ++j) {
			clCp.push_back(cl[j]);
			if(Minisat::var(cl[j])+1 > nVarsAtCall) defined = false;
		}
		if(defined) this->learnts.push_back(clCp);
	}
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
	BuiltInSatSolverNG::addVariables(nVars);
}

void BuiltInMssSolverNG::addVariables(int nVars, bool auxVar) {
	BuiltInSatSolverNG::addVariables(nVars, auxVar);
}


bool BuiltInMssSolverNG::addClause(std::vector<int> &clause) {
	return BuiltInSatSolverNG::addClause(clause);
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
	return BuiltInSatSolverNG::isPropagatedAtDecisionLvlZero(lit);
}

bool BuiltInMssSolverNG::isPropagatedAtDecisionLvlZero(int lit, std::vector<int> assumps) {
	return BuiltInSatSolverNG::isPropagatedAtDecisionLvlZero(lit, assumps);
}


bool BuiltInMssSolverNG::computeModel() {
	return BuiltInSatSolverNG::computeModel();
}


bool BuiltInMssSolverNG::computeModel(std::vector<int> &assumps) {
	return BuiltInSatSolverNG::computeModel(assumps);
}


void BuiltInMssSolverNG::computeAllModels(std::function<void(std::vector<bool>&)> callback) {
	BuiltInSatSolverNG::computeAllModels(callback);
}


void BuiltInMssSolverNG::computeAllModels(std::function<void(std::vector<bool>&)> callback, std::vector<int> &assumps) {
	BuiltInSatSolverNG::computeAllModels(callback, assumps);
}


bool BuiltInMssSolverNG::hasAModel() {
	return BuiltInSatSolverNG::hasAModel();
}


std::vector<bool>& BuiltInMssSolverNG::getModel() {
	return BuiltInSatSolverNG::getModel();
}


std::vector<std::vector<bool> >& BuiltInMssSolverNG::getModels() {
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