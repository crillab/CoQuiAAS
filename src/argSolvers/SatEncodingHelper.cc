/*
 * SatEncodingHelper.cc
 *
 *  Created on: 21 juil. 2016
 *      Author: lonca
 */

#include "SatEncodingHelper.h"


using namespace CoQuiAAS;


SatEncodingHelper::SatEncodingHelper(std::shared_ptr<SatSolver> solver, Attacks &attacks, VarMap &varMap) : solver(solver), attacks(attacks), varMap(varMap) {
	this->nbVars = varMap.nVars();
	solver->addVariables(this->nbVars);
}


int SatEncodingHelper::reserveVars(int n) {
	int ret = this->nbVars + 1;
	solver->addVariables(n, true);
	this->nbVars += n;
	return ret;
}


int SatEncodingHelper::reserveDisjunctionVars() {
	return reserveVars(varMap.intVars().size());
}


void SatEncodingHelper::createAttackersDisjunctionVars(int startId) {
	std::vector<int, std::allocator<int> >& vars = varMap.intVars();
	Minisat::vec<Minisat::Lit> binaryClause, naryClause;
	std::vector<int> binaryCl, completeCl;
	for(std::vector<int>::iterator itVars = vars.begin() ; itVars != vars.end(); ++itVars) {
		int var = *itVars;
		std::vector<int, std::allocator<int> >* attacksToCurrentVar = this->attacks.getAttacksTo(var);
		binaryCl.push_back(-var);
		binaryCl.push_back(-(var+startId-1));
		solver->addClause(binaryCl); // (-a \lor -Pa)
		binaryCl.clear();
		completeCl.push_back(-(var+startId-1));
		for(std::vector<int>::iterator itAttackers = attacksToCurrentVar->begin(); itAttackers != attacksToCurrentVar->end(); ++itAttackers) {
			int attacker = *itAttackers;
			binaryCl.push_back(var+startId-1);
			binaryCl.push_back(-attacker);
			solver->addClause(binaryCl); // Pa \lor -b
			binaryCl.clear();
			completeCl.push_back(attacker);
		}
		solver->addClause(completeCl); // -Pa \lor b1 \lor ... \lor bp
		completeCl.clear();
	}
}


void SatEncodingHelper::createCompleteEncodingConstraints(int startId) {
	std::vector<int> binaryCl, completeCl;
	std::vector<int, std::allocator<int> >& vars = varMap.intVars();
	for(std::vector<int>::iterator itVars = vars.begin() ; itVars != vars.end(); ++itVars) {
		int var = *itVars;
		std::vector<int, std::allocator<int> >* attacksToCurrentVar = attacks.getAttacksTo(var);
		completeCl.push_back(var);
		for(std::vector<int>::iterator itAttackers = attacksToCurrentVar->begin(); itAttackers != attacksToCurrentVar->end(); ++itAttackers) {
			int attacker = *itAttackers;
			binaryCl.push_back(-var);
			binaryCl.push_back(attacker+startId-1); // -a \lor Pb
			solver->addClause(binaryCl);
			binaryCl.clear();
			completeCl.push_back(-(attacker+startId-1));
		}
		solver->addClause(completeCl);
		completeCl.clear();
	}
}


void SatEncodingHelper::createConflictFreenessEncodingConstraints(int startId) {
	std::vector<int> binaryCl;
	std::vector<int, std::allocator<int> >& vars = varMap.intVars();
	for(std::vector<int>::iterator itVars = vars.begin() ; itVars != vars.end(); ++itVars) {
		int var = *itVars;
		std::vector<int, std::allocator<int> >* attacksToCurrentVar = attacks.getAttacksTo(var);
		for(std::vector<int>::iterator itAttackers = attacksToCurrentVar->begin(); itAttackers != attacksToCurrentVar->end(); ++itAttackers) {
			int attacker = *itAttackers;
			binaryCl.push_back(-var);
			binaryCl.push_back(-attacker); // -a \lor -b
			solver->addClause(binaryCl);
			binaryCl.clear();
		}
	}
}


void SatEncodingHelper::createStableEncodingConstraints() {
	std::vector<int> binaryCl, completeCl;
	std::vector<int, std::allocator<int> >& vars = varMap.intVars();
	for(std::vector<int>::iterator itVars = vars.begin() ; itVars != vars.end(); ++itVars) {
		int var = *itVars;
		completeCl.push_back(var);
		std::vector<int, std::allocator<int> >* attacksToCurrentVar = attacks.getAttacksTo(var);
		for(std::vector<int>::iterator itAttackers = attacksToCurrentVar->begin(); itAttackers != attacksToCurrentVar->end(); ++itAttackers) {
			int attacker = *itAttackers;
			binaryCl.push_back(-(var));
			if(var != attacker) binaryCl.push_back(-(attacker));
			solver->addClause(binaryCl); // -a \lor -b
			binaryCl.clear();
			completeCl.push_back(attacker);
		}
		solver->addClause(completeCl); // a \lor b1 \lor ... \lor bp
		completeCl.clear();
	}
}


void SatEncodingHelper::createStableEncodingConstraints(int startId) {
	std::vector<int> cl;
	std::vector<int, std::allocator<int> >& vars = varMap.intVars();
	for(std::vector<int>::iterator itVars = vars.begin() ; itVars != vars.end(); ++itVars) {
		int var = *itVars;
		std::vector<int, std::allocator<int> >* attacksToCurrentVar = attacks.getAttacksTo(var);
		if(attacksToCurrentVar->size() == 0) {
			cl.push_back(var);
			solver->addClause(cl); // a
			cl.clear();
			cl.push_back(-(var+startId-1));
			solver->addClause(cl); // -Pa
			cl.clear();
			continue;
		}
		cl.push_back(var);
		cl.push_back(var+startId-1);
		solver->addClause(cl); // a \lor Pa
		cl.clear();
		cl.push_back(-var);
		cl.push_back(var+startId-1);
		solver->addClause(cl); // -a \lor -Pa
		cl.clear();
	}
}


SatEncodingHelper::~SatEncodingHelper() {}

