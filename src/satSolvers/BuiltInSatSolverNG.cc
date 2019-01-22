/*
 * BuiltInSatSolver.cc
 *
 *  Created on: 21 juil. 2016
 *      Author: lonca
 */


#include "BuiltInSatSolverNG.h"


using namespace CoQuiAAS;


BuiltInSatSolverNG::BuiltInSatSolverNG() {
	this->setBlockingClauseFunction([this](std::vector<bool>& model) -> std::vector<int> {
		vector<int> intCl;
		for(int i=0; i<this->realNVars; ++i) {
			intCl.push_back(model[i] ? -(i+1) : i+1);
		}
		return intCl;
	});
}


void BuiltInSatSolverNG::buildSolver() {
	vec<Lit> ps;
	if(!this->solver) {
		this->solver = new MiniSatSolver();
		for(int i=0; i<this->formula.nVars(); ++i) {
			this->solver->newVar();
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
	this->newFormula = WCNF();
}


void BuiltInSatSolverNG::addVariables(int nVars) {
	addVariables(nVars, false);
}


void BuiltInSatSolverNG::addVariables(int nVars, bool auxVar) {
	for(int i=0; i<nVars; ++i) this->newFormula.newVar();
	if(!auxVar) this->realNVars += nVars;
}


bool BuiltInSatSolverNG::addClause(std::vector<int> &clause) {
	CMP::vec<CMP::Lit> cmpCl;
	toCmpClause(clause, cmpCl);
	this->newFormula.addHard(cmpCl);
	return true;
}


int BuiltInSatSolverNG::addSelectedClause(std::vector<int> &clause) {
	addVariables(1, true);
	int selector = this->formula.nVars() + this->newFormula.nVars();
	clause.push_back(-selector);
	addClause(clause);
	return selector;
}


std::vector<int>& BuiltInSatSolverNG::propagatedAtDecisionLvlZero() {
	std::vector<int> assumps;
	return propagatedAtDecisionLvlZero(assumps);
}


std::vector<int>& BuiltInSatSolverNG::propagatedAtDecisionLvlZero(std::vector<int> assumps) {
	buildSolver();
	for (int c = solver->slv->trail.size()-1; c >= 0; c--) solver->slv->assigns[Minisat::var(solver->slv->trail[c])] = Minisat::lbool((uint8_t)2);
    solver->slv->qhead = 0;
    solver->slv->trail.shrink(solver->slv->trail.size());
    solver->slv->trail_lim.shrink(solver->slv->trail_lim.size());
	propagated.clear();
	solver->slv->useAsCompleteSolver();
	for(unsigned int i=0; i<assumps.size(); ++i) {
		int assump = assumps[i];
		solver->slv->enqueue(assump > 0 ? Minisat::mkLit(assump-1) : ~Minisat::mkLit(-assump-1));
	}
	solver->slv->propagate();
	for (int i = 0; i < solver->slv->nAssigns(); i++) {
		if (!Minisat::sign(solver->slv->trail[i])) {
			propagated.push_back(Minisat::var(solver->slv->trail[i])+1);
		}
	}
	return propagated;
}


bool BuiltInSatSolverNG::isPropagatedAtDecisionLvlZero(int lit) {
	std::vector<int> assumps;
	return isPropagatedAtDecisionLvlZero(lit, assumps);
}


bool BuiltInSatSolverNG::isPropagatedAtDecisionLvlZero(int lit, std::vector<int> assumps) {
	std::vector<int>& propagated = propagatedAtDecisionLvlZero(assumps);
	for(std::vector<int>::iterator it = propagated.begin(); it != propagated.end(); ++it) {
		if(*it == lit) return true;
	}
	return false;
}


void BuiltInSatSolverNG::clearModels() {
	this->models.clear();
	this->blockingSelectors.clear();
}


bool BuiltInSatSolverNG::computeModel() {
	std::vector<int> assumps;
	return computeModel(assumps);
}


bool BuiltInSatSolverNG::computeModel(vector<int> &assumps) {
	return computeModel(assumps, true);
}


bool BuiltInSatSolverNG::computeModel(vector<int> &assumps, bool clearModelVec) {
	buildSolver();
	if(clearModelVec) clearModels();
	Minisat::vec<Minisat::Lit> minisatAssumps;
	intClauseToBuiltInClause(assumps, minisatAssumps);
	Minisat::lbool ret = solver->slv->solveLimited(minisatAssumps);
	if(ret == MINISAT_LBOOL_TRUE) {
		extractBuiltInSolverModel();
		return true;
	} else if (ret == MINISAT_LBOOL_UNDEF) {
		std::cerr << "ERR:: Minisat returned LBOOL_UNDEF" << std::endl;
		exit(127);
	}
	return false;
}


void BuiltInSatSolverNG::extractBuiltInSolverModel() {
	std::vector<bool> newModel;
	for(int i=0; i<this->formula.nVars(); ++i) {
		newModel.push_back(solver->slv->model[i] == MINISAT_LBOOL_TRUE);
	}
	this->models.push_back(newModel);
}


void BuiltInSatSolverNG::computeAllModels(std::function<void(std::vector<bool>&)> callback) {
	std::vector<int> assumps;
	return computeAllModels(callback, assumps);
}


void BuiltInSatSolverNG::computeAllModels(std::function<void(std::vector<bool>&)> callback, std::vector<int> &assumps) {
	buildSolver();
	clearModels();
	for(;;) {
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


bool BuiltInSatSolverNG::hasAModel() {
	return this->models.size() > 0;
}


std::vector<bool>& BuiltInSatSolverNG::getModel() {
	return this->models[this->models.size()-1];
}


std::vector<std::vector<bool> >& BuiltInSatSolverNG::getModels() {
	return this->models;
}

void BuiltInSatSolverNG::resetModels() {
	this->models.clear();
}

void BuiltInSatSolverNG::toCmpClause(std::vector<int> &clause, CMP::vec<CMP::Lit>& cmpCl) {
	int size = clause.size();
	cmpCl.capacity(size);
	cmpCl.clear();
	for(int i=0; i<size; ++i) {
		int lit = clause[i];
		cmpCl.push(lit > 0 ? CMP::mkLit(lit-1) : ~CMP::mkLit(-lit-1));
	}
}

BuiltInSatSolverNG::~BuiltInSatSolverNG() {
	// delete this->solver;
}

