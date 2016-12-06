/*
 * BuiltInSatSolver.cc
 *
 *  Created on: 21 juil. 2016
 *      Author: lonca
 */


#include "BuiltInSatSolver.h"


using namespace CoQuiAAS;


BuiltInSatSolver::BuiltInSatSolver() {
	this->nVars = 0;
	this->nCstrs = 0;
}


void BuiltInSatSolver::addVariables(int nVars) {
	for(int i=0; i<nVars; ++i) {
		solver.newVar();
	}
	this->nVars += nVars;
}


bool BuiltInSatSolver::addClause(std::vector<int> &clause) {
	Minisat::vec<Minisat::Lit> minisatCl;
	intClauseToBuiltInClause(clause, minisatCl);
	bool added = solver.addClause_(minisatCl, true);
	if(added) ++this->nCstrs;
	return added;
}


int BuiltInSatSolver::addSelectedClause(std::vector<int> &clause) {
	addVariables(1);
	int selector = this->nVars;
	clause.push_back(-selector);
	addClause(clause);
	return selector;
}


std::vector<int>& BuiltInSatSolver::propagatedAtDecisionLvlZero() {
	solver.useAsCompleteSolver();
	solver.propagate();
	for (int i = 0; i < solver.nAssigns(); i++) {
		if (!Minisat::sign(solver.trail[i])) {
			propagated.push_back(Minisat::var(solver.trail[i])+1);
		}
	}
	return propagated;
}


bool BuiltInSatSolver::isPropagatedAtDecisionLvlZero(int lit) {
	std::vector<int>& propagated = propagatedAtDecisionLvlZero();
	for(std::vector<int>::iterator it = propagated.begin(); it != propagated.end(); ++it) {
		if(*it == lit) return true;
	}
	return false;
}


void BuiltInSatSolver::computeModel() {
	std::vector<int> assumps;
	computeModel(assumps);
}


void BuiltInSatSolver::computeModel(vector<int> &assumps) {
	computeModel(assumps, true);
}


void BuiltInSatSolver::computeModel(vector<int> &assumps, bool clearModelVec) {
	if(clearModelVec) this->models.clear();
	solver.useAsCompleteSolver();
	Minisat::vec<Minisat::Lit> minisatAssumps;
	intClauseToBuiltInClause(assumps, minisatAssumps);
	Minisat::lbool ret = solver.solveLimited(minisatAssumps);
	if(ret == MINISAT_LBOOL_TRUE) {
		extractBuiltInSolverModel();
	} else if (ret == MINISAT_LBOOL_UNDEF) {
		std::cerr << "ERR:: Minisat returned LBOOL_UNDEF" << std::endl;
		exit(127);
	}
}


void BuiltInSatSolver::extractBuiltInSolverModel() {
	std::vector<bool> newModel;
	for(int i=0; i<this->nVars; ++i) {
		newModel.push_back(solver.model[i] == MINISAT_LBOOL_TRUE);
	}
	this->models.push_back(newModel);
}


void BuiltInSatSolver::computeAllModels() {
	std::vector<int> assumps;
	return computeAllModels(assumps);
}


void BuiltInSatSolver::computeAllModels(vector<int> &assumps) {
	this->models.clear();
	unsigned int nbModels = 0;
	std::vector<int> blockingSelectors;
	this->models.clear();
	for(;;) {
		computeModel(assumps, false);
		solver.bigRestart();
		if(this->models.size() > nbModels) {
			int sel = addBlockingClause();
			blockingSelectors.push_back(sel);
			assumps.push_back(sel);
			++nbModels;
		} else {
			break;
		}
	}
	for(int i=0; i<(int) nbModels; ++i) {
		std::vector<int> cl;
		cl.push_back(-blockingSelectors[i]);
		addClause(cl);
	}
}


int BuiltInSatSolver::addBlockingClause() {
	vector<bool> model = this->models[this->models.size() - 1];
	vector<int> intCl;
	for(int i=0; i<this->nVars; ++i) {
		intCl.push_back(model[i] ? -(i+1) : i+1);
	}
	return addSelectedClause(intCl);
}


bool BuiltInSatSolver::hasAModel() {
	return this->models.size() > 0;
}


std::vector<bool>& BuiltInSatSolver::getModel() {
	return this->models[0];
}


std::vector<std::vector<bool> >& BuiltInSatSolver::getModels() {
	return this->models;
}


BuiltInSatSolver::~BuiltInSatSolver() {}

