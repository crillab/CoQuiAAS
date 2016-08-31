/*
 * SatEncodingHelper.cc
 *
 *  Created on: 21 juil. 2016
 *      Author: lonca
 */

#include "SatEncodingHelper.h"


using namespace CoQuiAAS;


SatEncodingHelper::SatEncodingHelper(SatSolver &solver, Attacks &attacks, VarMap &varMap) : solver(solver), attacks(attacks), varMap(varMap) {
	this->nbVars = 0;
	solver.addVariables(varMap.nVars());
}


void SatEncodingHelper::createAttackersDisjunctionVars(int startId) {
	std::vector<int, std::allocator<int> >* vars = varMap.intVars();
	int nbArgs = vars->size();
	solver.addVariables(nbArgs);
	Minisat::vec<Minisat::Lit> binaryClause, naryClause;
	std::vector<int> binaryCl, completeCl;
	for(std::vector<int>::iterator itVars = vars->begin() ; itVars != vars->end(); ++itVars) {
		int var = *itVars;
		std::vector<int, std::allocator<int> >* attacksToCurrentVar = this->attacks.getAttacksTo(var);
		binaryCl.push_back(-var);
		binaryCl.push_back(-(var+startId));
		solver.addClause(binaryCl); // (-a \lor -Pa)
		binaryCl.clear();
		completeCl.push_back(-(var+startId));
		for(std::vector<int>::iterator itAttackers = attacksToCurrentVar->begin(); itAttackers != attacksToCurrentVar->end(); ++itAttackers) {
			int attacker = *itAttackers;
			binaryCl.push_back(var+startId);
			binaryCl.push_back(-attacker);
			solver.addClause(binaryCl); // Pa \lor -b
			binaryCl.clear();
			completeCl.push_back(attacker);
		}
		solver.addClause(completeCl); // -Pa \lor b1 \lor ... \lor bp
		completeCl.clear();
		++this->nbVars;
	}
}


void SatEncodingHelper::createCompleteEncodingConstraints(int attackersDisjunctionFirstVar) {
	std::vector<int> binaryCl, completeCl;
	std::vector<int, std::allocator<int> >* vars = varMap.intVars();
	for(std::vector<int>::iterator itVars = vars->begin() ; itVars != vars->end(); ++itVars) {
		int var = *itVars;
		std::vector<int, std::allocator<int> >* attacksToCurrentVar = attacks.getAttacksTo(var);
		completeCl.push_back(var);
		for(std::vector<int>::iterator itAttackers = attacksToCurrentVar->begin(); itAttackers != attacksToCurrentVar->end(); ++itAttackers) {
			int attacker = *itAttackers;
			binaryCl.push_back(-var);
			binaryCl.push_back(attacker+attackersDisjunctionFirstVar); // -a \lor Pb
			solver.addClause(binaryCl);
			binaryCl.clear();
			completeCl.push_back(-(attacker+attackersDisjunctionFirstVar));
		}
		solver.addClause(completeCl);
		completeCl.clear();
	}
}


void SatEncodingHelper::createStableEncodingConstraints() {
	std::vector<int> binaryCl, completeCl;
	std::vector<int, std::allocator<int> >* vars = varMap.intVars();
	for(std::vector<int>::iterator itVars = vars->begin() ; itVars != vars->end(); ++itVars) {
		int var = *itVars;
		completeCl.push_back(var);
		std::vector<int, std::allocator<int> >* attacksToCurrentVar = attacks.getAttacksTo(var);
		for(std::vector<int>::iterator itAttackers = attacksToCurrentVar->begin(); itAttackers != attacksToCurrentVar->end(); ++itAttackers) {
			int attacker = *itAttackers;
			if(var == attacker) continue;
			binaryCl.push_back(-(var));
			binaryCl.push_back(-(attacker));
			solver.addClause(binaryCl); // -a \lor -b
			binaryCl.clear();
			completeCl.push_back(attacker);
		}
		solver.addClause(completeCl); // a \lor b1 \lor ... \lor bp
		completeCl.clear();
	}
}


void SatEncodingHelper::createStableEncodingConstraints(int attackersDisjunctionFirstVar) {
	std::vector<int> cl;
	std::vector<int, std::allocator<int> >* vars = varMap.intVars();
	for(std::vector<int>::iterator itVars = vars->begin() ; itVars != vars->end(); ++itVars) {
		int var = *itVars;
		std::vector<int, std::allocator<int> >* attacksToCurrentVar = attacks.getAttacksTo(var);
		if(attacksToCurrentVar->size() == 0) {
			cl.push_back(var);
			solver.addClause(cl); // a
			cl.clear();
			cl.push_back(-(var+attackersDisjunctionFirstVar));
			solver.addClause(cl); // -Pa
			cl.clear();
			continue;
		}
		cl.push_back(var);
		cl.push_back(var+attackersDisjunctionFirstVar);
		solver.addClause(cl); // a \lor Pa
		cl.clear();
		cl.push_back(-var);
		cl.push_back(var+attackersDisjunctionFirstVar);
		solver.addClause(cl); // -a \lor -Pa
		cl.clear();
	}
}


SatEncodingHelper::~SatEncodingHelper() {}

