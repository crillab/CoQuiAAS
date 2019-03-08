/*
 * BuiltInMssSolverNG.cc
 *
 *  Created on: 21 juil. 2016
 *      Author: lonca
 */


#include "BuiltInMssSolverNGGlucose.h"


using namespace CoQuiAAS;


BuiltInMssSolverNGGlucose::BuiltInMssSolverNGGlucose() : BuiltInSatSolverNGGlucose() {
	this->nSoftCstrs = 0;
}


void BuiltInMssSolverNGGlucose::addSoftClause(std::vector<int> &clause) {
	CMP::vec<CMP::Lit> cmpCl;
	toCmpClause(clause, cmpCl);
	this->newFormula.addSoft(cmpCl, this->nSoftCstrs);
	++this->nSoftCstrs;
}


void BuiltInMssSolverNGGlucose::clearMss() {
	this->mss.clear();
}


bool BuiltInMssSolverNGGlucose::computeMss() {
	std::vector<int> assumps;
	return computeMss(assumps);
}

bool BuiltInMssSolverNGGlucose::computeMss(std::vector<int> &assumps) {
	computeSomeMsses(NULL, assumps, 1);
	return true;
}


void BuiltInMssSolverNGGlucose::computeAllMss(std::function<void(std::vector<int>&, std::vector<bool>&)> callback) {
	std::vector<int> assumps;
	computeAllMss(callback, assumps);
}


void BuiltInMssSolverNGGlucose::computeAllMss(std::function<void(std::vector<int>&, std::vector<bool>&)> callback, std::vector<int> &assumps) {
	computeSomeMsses(callback, assumps, INT32_MAX);
}


void BuiltInMssSolverNGGlucose::buildEnumerator(int maxCount) {
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
	//for(unsigned int i=0; i<this->learnts.size(); ++i) ((MiniSatSolver*) this->mcsEnumerator->exttor->getSatSolver())->slv->restoreLearnt(learnts[i]);
}


void BuiltInMssSolverNGGlucose::computeSomeMsses(std::function<void(std::vector<int>&, std::vector<bool>&)> callback, std::vector<int> &assumps, int maxCount) {
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
		this->mss.push_back(mss);
		this->models.push_back(mod);
		if(callback) callback(mss, mod);
		if(this->shouldStopMssEnum) this->mcsEnumerator->stopEnum();
	});
	storeLearnts(nVars);
}


void BuiltInMssSolverNGGlucose::storeLearnts(int nVarsAtCall) {
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


bool BuiltInMssSolverNGGlucose::hasAMss() {
	return this->mss.size() > 0;
}


std::vector<int>& BuiltInMssSolverNGGlucose::getMss() {
	return this->mss[this->mss.size()-1];
}


std::vector<std::vector<int> >& BuiltInMssSolverNGGlucose::getAllMss() {
	return this->mss;
}


BuiltInMssSolverNGGlucose::~BuiltInMssSolverNGGlucose() {
	delete this->solver;
}

void BuiltInMssSolverNGGlucose::addVariables(int nVars) {
	BuiltInSatSolverNGGlucose::addVariables(nVars);
}

void BuiltInMssSolverNGGlucose::addVariables(int nVars, bool auxVar) {
	BuiltInSatSolverNGGlucose::addVariables(nVars, auxVar);
}


bool BuiltInMssSolverNGGlucose::addClause(std::vector<int> &clause) {
	return BuiltInSatSolverNGGlucose::addClause(clause);
}


int BuiltInMssSolverNGGlucose::addSelectedClause(std::vector<int> &clause) {
	return BuiltInSatSolverNGGlucose::addSelectedClause(clause);
}


std::vector<int>& BuiltInMssSolverNGGlucose::propagatedAtDecisionLvlZero() {
	return BuiltInSatSolverNGGlucose::propagatedAtDecisionLvlZero();
}


std::vector<int>& BuiltInMssSolverNGGlucose::propagatedAtDecisionLvlZero(std::vector<int> assumps) {
	return BuiltInSatSolverNGGlucose::propagatedAtDecisionLvlZero(assumps);
}


bool BuiltInMssSolverNGGlucose::isPropagatedAtDecisionLvlZero(int lit) {
	return BuiltInSatSolverNGGlucose::isPropagatedAtDecisionLvlZero(lit);
}

bool BuiltInMssSolverNGGlucose::isPropagatedAtDecisionLvlZero(int lit, std::vector<int> assumps) {
	return BuiltInSatSolverNGGlucose::isPropagatedAtDecisionLvlZero(lit, assumps);
}


bool BuiltInMssSolverNGGlucose::computeModel() {
	return BuiltInSatSolverNGGlucose::computeModel();
}


bool BuiltInMssSolverNGGlucose::computeModel(std::vector<int> &assumps) {
	return BuiltInSatSolverNGGlucose::computeModel(assumps);
}


void BuiltInMssSolverNGGlucose::computeAllModels(std::function<void(std::vector<bool>&)> callback) {
	BuiltInSatSolverNGGlucose::computeAllModels(callback);
}


void BuiltInMssSolverNGGlucose::computeAllModels(std::function<void(std::vector<bool>&)> callback, std::vector<int> &assumps) {
	BuiltInSatSolverNGGlucose::computeAllModels(callback, assumps);
}

void BuiltInMssSolverNGGlucose::computeAllModels(std::function<void(std::vector<bool>&)> callback, std::vector<int> &assumps, std::vector<bool> knownModel){
	BuiltInMssSolverNGGlucose::computeAllModels(callback, assumps, knownModel);
}


bool BuiltInMssSolverNGGlucose::hasAModel() {
	return BuiltInSatSolverNGGlucose::hasAModel();
}


std::vector<bool>& BuiltInMssSolverNGGlucose::getModel() {
	return BuiltInSatSolverNGGlucose::getModel();
}


std::vector<std::vector<bool> >& BuiltInMssSolverNGGlucose::getModels() {
	return BuiltInSatSolverNGGlucose::getModels();
}

void BuiltInMssSolverNGGlucose::resetAllMss() {
	this->mss.clear();
}

void BuiltInMssSolverNGGlucose::resetModels() {
	this->models.clear();
}

void BuiltInMssSolverNGGlucose::stopMssEnum(){
	this->shouldStopMssEnum = true;
}

void BuiltInMssSolverNGGlucose::toCmpClause(std::vector<int> &clause, CMP::vec<CMP::Lit>& cmpCl) {
	for(std::vector<int>::iterator it = clause.begin(); it != clause.end(); ++it) {
		int lit = *it;
		CMP::Lit minisatLit = lit > 0 ? CMP::mkLit(lit-1) : ~CMP::mkLit(-lit-1);
		cmpCl.push(minisatLit);
	}
}