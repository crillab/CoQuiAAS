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
		std::cout << (solver.model[i] == MINISAT_LBOOL_TRUE) << " ";
	}
	std::cout << std::endl;
	this->models.push_back(newModel);
}


void BuiltInSatSolver::computeAllModels() {
	std::vector<int> assumps;
	return computeAllModels(assumps);
}


void BuiltInSatSolver::computeAllModels(vector<int> &assumps) {
	unsigned int nbModels = 0;
	this->models.clear();
	for(;;) {
		std::cout << "nclauses: " << solver.nClauses() << std::endl;
		computeModel(assumps);
		solver.bigRestart();
		if(this->models.size() > nbModels) {
			if(!addBlockingClause()) break;
			++nbModels;
		} else {
			break;
		}
	}
}


bool BuiltInSatSolver::addBlockingClause() {
	vector<bool> model = this->models[this->models.size() - 1];
	vector<int> intCl;
	for(int i=0; i<this->nVars; ++i) {
		intCl.push_back(model[i] ? -(i+1) : i+1);
		std::cout << (model[i] ? 0 : 1) << " ";
	}
	std::cout << "-" << std::endl;
	return addClause(intCl);
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

