/*
 * BuiltInSatSolver.cc
 *
 *  Created on: 21 juil. 2016
 *      Author: lonca
 */


#include "BuiltInSatSolverNGGlucose.h"


using namespace CoQuiAAS;


BuiltInSatSolverNGGlucose::BuiltInSatSolverNGGlucose() {
	this->setBlockingClauseFunction([this](std::vector<bool>& model) -> std::vector<int> {
		vector<int> intCl;
		for(int i=0; i<this->realNVars; ++i) {
			intCl.push_back(model[i] ? -(i+1) : i+1);
		}
		return intCl;
	});
	this->cmpCl.capacity(1<<10);
}


void BuiltInSatSolverNGGlucose::buildSolver() {
	vec<Lit> ps;
	if(!this->solver) {
		Logger::getInstance()->debug("building a new instance of Glucose with %d vars and %d constraints", this->formula.nVars()+this->newFormula.nVars(), this->formula.nHards()+this->newFormula.nHards());
		this->solver = new GlucoseSolver();
		this->solver->slv->phase_saving = 0;
		for(int i=0; i<this->formula.nVars(); ++i) {
			this->solver->newVar(true, this->solver->nVars()+1 <= this->realNVars);
		}
		for(int i=0; i<this->formula.nHards() ;++i) {
			this->formula.getHard(i, ps);
			this->solver->addClause(ps);
		}
	}
	for(int i=0; i<this->newFormula.nVars(); ++i) {
		this->formula.newVar();
		this->solver->newVar();
	}
	for(int i=0; i<this->newFormula.nHards() ;++i) {
		this->newFormula.getHard(i, ps);
		this->formula.addHard(ps);
		this->solver->addClause(ps);
	}
	for(int i=0; i<this->newFormula.nSofts() ;++i) {
		this->newFormula.getSoft(i, ps);
		this->formula.addSoft(ps, this->formula.nSofts());
	}
	this->newFormula = WCNF();
}


void BuiltInSatSolverNGGlucose::addVariables(int nVars) {
	addVariables(nVars, false);
}


void BuiltInSatSolverNGGlucose::addVariables(int nVars, bool auxVar) {
	for(int i=0; i<nVars; ++i) this->newFormula.newVar();
	if(!auxVar) this->realNVars += nVars;
}


bool BuiltInSatSolverNGGlucose::addClause(std::vector<int> &clause) {
	toCmpClause(clause, cmpCl);
	this->newFormula.addHard(cmpCl);
	return true;
}


int BuiltInSatSolverNGGlucose::addSelectedClause(std::vector<int> &clause) {
	addVariables(1, true);
	int selector = this->formula.nVars() + this->newFormula.nVars();
	clause.push_back(-selector);
	addClause(clause);
	return selector;
}


std::vector<int>& BuiltInSatSolverNGGlucose::propagatedAtDecisionLvlZero() {
	std::vector<int> assumps;
	return propagatedAtDecisionLvlZero(assumps);
}


std::vector<int>& BuiltInSatSolverNGGlucose::propagatedAtDecisionLvlZero(std::vector<int> assumps) {
	return propagatedAtDecisionLvlZero(assumps, false);
}


std::vector<int>& BuiltInSatSolverNGGlucose::propagatedAtDecisionLvlZero(std::vector<int> assumps, bool includeNegLits) {
	buildSolver();
	resetSolverState();
	propagated.clear();
	solver->slv->useAsCompleteSolver();
	solver->slv->newDecisionLevel();
	for(unsigned int i=0; i<assumps.size(); ++i) {
		int assump = assumps[i];
		solver->slv->enqueue(assump > 0 ? Glucose::mkLit(assump-1) : ~Glucose::mkLit(-assump-1));
	}
	solver->slv->propagate();
	for (int i = 0; i < solver->slv->nAssigns(); i++) {
		if (!Glucose::sign(solver->slv->trail[i])) {
			propagated.push_back(Glucose::var(solver->slv->trail[i])+1);
		} else if(includeNegLits) {
			propagated.push_back(-Glucose::var(solver->slv->trail[i])-1);
		}
	}
	solver->slv->cancelUntil(0);
	return propagated;
}


void BuiltInSatSolverNGGlucose::resetSolverState() {
	for (int c = solver->slv->trail.size()-1; c >= 0; c--) solver->slv->assigns[Glucose::var(solver->slv->trail[c])] = Glucose::lbool((uint8_t)2);
    solver->slv->qhead = 0;
    solver->slv->trail.shrink(solver->slv->trail.size());
    solver->slv->trail_lim.shrink(solver->slv->trail_lim.size());
}


bool BuiltInSatSolverNGGlucose::isPropagatedAtDecisionLvlZero(int lit) {
	std::vector<int> assumps;
	return isPropagatedAtDecisionLvlZero(lit, assumps);
}


bool BuiltInSatSolverNGGlucose::isPropagatedAtDecisionLvlZero(int lit, std::vector<int> assumps) {
	std::vector<int>& propagated = propagatedAtDecisionLvlZero(assumps);
	for(std::vector<int>::iterator it = propagated.begin(); it != propagated.end(); ++it) {
		if(*it == lit) return true;
	}
	return false;
}


void BuiltInSatSolverNGGlucose::clearModels() {
	this->models.clear();
	this->blockingSelectors.clear();
}


bool BuiltInSatSolverNGGlucose::computeModel() {
	std::vector<int> assumps;
	return computeModel(assumps);
}


bool BuiltInSatSolverNGGlucose::computeModel(vector<int> &assumps) {
	return computeModel(assumps, true);
}


bool BuiltInSatSolverNGGlucose::computeModel(vector<int> &assumps, bool clearModelVec) {
	buildSolver();
	if(clearModelVec) clearModels();
	Glucose::vec<Glucose::Lit> minisatAssumps;
	intClauseToBuiltInClause(assumps, minisatAssumps);
	Glucose::lbool ret = solver->slv->solveLimited(minisatAssumps);
	if(ret == GLUCOSE_LBOOL_TRUE) {
		extractBuiltInSolverModel();
		return true;
	} else if (ret == GLUCOSE_LBOOL_UNDEF) {
		std::cerr << "ERR:: Minisat returned LBOOL_UNDEF" << std::endl;
		exit(127);
	}
	return false;
}


void BuiltInSatSolverNGGlucose::extractBuiltInSolverModel() {
	std::vector<bool> newModel;
	for(int i=0; i<this->formula.nVars(); ++i) {
		newModel.push_back(solver->slv->model[i] == GLUCOSE_LBOOL_TRUE);
	}
	this->models.push_back(newModel);
}


void BuiltInSatSolverNGGlucose::computeAllModels(std::function<void(std::vector<bool>&)> callback) {
	std::vector<int> assumps;
	return computeAllModels(callback, assumps);
}


void BuiltInSatSolverNGGlucose::computeAllModels(std::function<void(std::vector<bool>&)> callback, std::vector<int> &assumps) {
	this->shouldStopModelEnum = false;
	buildSolver();
	clearModels();
	for(;;) {
		if(this->shouldStopModelEnum) break;
		bool newModel = computeModel(assumps, false);
		if(!newModel) break;
        std::vector<bool> model = this->models[this->models.size()-1];
        if(callback) callback(model);
        std::vector<int> blockingCl = this->blockingClauseFunction(model);
        int sel = addSelectedClause(blockingCl);
		blockingSelectors.push_back(sel);
		assumps.push_back(sel);
	}
	for(int i=0; i<(int) this->models.size(); ++i) {
		std::vector<int> cl;
		cl.push_back(-blockingSelectors[i]);
		addClause(cl);
	}
}


void BuiltInSatSolverNGGlucose::computeAllModels(std::function<void(std::vector<bool>&)> callback, std::vector<int> &assumps, std::vector<bool> knownModel) {
	this->shouldStopModelEnum = false;
	buildSolver();
	clearModels();
	this->models.push_back(knownModel);
	if(callback) callback(knownModel);
    std::vector<int> blockingCl = this->blockingClauseFunction(knownModel);
    int sel = addSelectedClause(blockingCl);
	blockingSelectors.push_back(sel);
	assumps.push_back(sel);
	for(;;) {
		if(this->shouldStopModelEnum) break;
		bool newModel = computeModel(assumps, false);
		if(!newModel) break;
        std::vector<bool> model = this->models[this->models.size()-1];
        if(callback) callback(model);
        std::vector<int> blockingCl = this->blockingClauseFunction(model);
        int sel = addSelectedClause(blockingCl);
		blockingSelectors.push_back(sel);
		assumps.push_back(sel);
	}
	for(int i=0; i<(int) this->models.size(); ++i) {
		std::vector<int> cl;
		cl.push_back(-blockingSelectors[i]);
		addClause(cl);
	}
}


bool BuiltInSatSolverNGGlucose::hasAModel() {
	return this->models.size() > 0;
}


std::vector<bool>& BuiltInSatSolverNGGlucose::getModel() {
	return this->models[this->models.size()-1];
}


std::vector<std::vector<bool> >& BuiltInSatSolverNGGlucose::getModels() {
	return this->models;
}

void BuiltInSatSolverNGGlucose::resetModels() {
	this->models.clear();
}

void BuiltInSatSolverNGGlucose::toCmpClause(std::vector<int> &clause, CMP::vec<CMP::Lit>& cmpCl) {
	cmpCl.shrink_(cmpCl.size());
	cmpCl.capacity(clause.size());
	for(std::vector<int>::iterator it = clause.begin(); it != clause.end(); ++it) cmpCl.push(*it > 0 ? CMP::mkLit(*it-1) : ~CMP::mkLit(-*it-1));
}

BuiltInSatSolverNGGlucose::~BuiltInSatSolverNGGlucose() {
	// delete this->solver;
}

