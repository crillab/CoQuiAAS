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
	int ret = reserveVars(varMap.intVars().size());
	reserveDynVars();
	return ret;
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
			for(unsigned int i=0; i<this->dynVars.size(); ++i) {
				if(std::get<0>(this->dynVars[i]) == attacker && std::get<1>(this->dynVars[i]) == var) {
					attacker = std::get<2>(this->dynVars[i]);
					break;
				}
			}
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
			int attackerDisjOfAttacker = attacker+startId-1;
			for(unsigned int i=0; i<this->dynVars.size(); ++i) {
				if(std::get<0>(this->dynVars[i]) == attacker && std::get<1>(this->dynVars[i]) == var) {
					attackerDisjOfAttacker = -std::get<3>(this->dynVars[i]);
					break;
				}
			}
			binaryCl.push_back(-var);
			binaryCl.push_back(attackerDisjOfAttacker); // -a \lor Pb
			solver->addClause(binaryCl);
			binaryCl.clear();
			completeCl.push_back(-attackerDisjOfAttacker);
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
	reserveDynVars();
	std::vector<int> binaryCl, completeCl;
	std::vector<int, std::allocator<int> >& vars = varMap.intVars();
	for(std::vector<int>::iterator itVars = vars.begin() ; itVars != vars.end(); ++itVars) {
		int var = *itVars;
		completeCl.push_back(var);
		std::vector<int, std::allocator<int> >* attacksToCurrentVar = attacks.getAttacksTo(var);
		for(std::vector<int>::iterator itAttackers = attacksToCurrentVar->begin(); itAttackers != attacksToCurrentVar->end(); ++itAttackers) {
			int attacker = *itAttackers;
			for(unsigned int i=0; i<this->dynVars.size(); ++i) {
				if(std::get<0>(this->dynVars[i]) == var && std::get<1>(this->dynVars[i]) == attacker) {
					attacker = std::get<2>(this->dynVars[i]);
					break;
				}
			}
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


void SatEncodingHelper::reserveDynVars() {
	auto dynAttacks = this->attacks.getDynAttacks();
	for(unsigned int i=0; i<dynAttacks.size(); ++i) {
		bool createNew = true;
		for(unsigned int j=0; j<this->dynVars.size(); ++j) {
			if(std::get<1>(dynAttacks[i]) == std::get<0>(dynVars[j]) && std::get<2>(dynAttacks[i]) == std::get<1>(dynVars[j])) {
				createNew = false;
				break;
			}
		}
		if(!createNew) continue;
		int fromVar = std::get<1>(dynAttacks[i]);
		int toVar = std::get<2>(dynAttacks[i]);
		int fromRepl = reserveVars(3);
		int notAttackedRepl = fromRepl + 1;
		int assump = fromRepl + 2;
		this->dynVars.push_back(std::make_tuple(fromVar, toVar, fromRepl, notAttackedRepl, assump));
		std::vector<int> cl;
		// fromRepl
		cl.push_back(-fromRepl);
		cl.push_back(fromVar);
		solver->addClause(cl);
		cl.clear();
		cl.push_back(-fromRepl);
		cl.push_back(assump);
		solver->addClause(cl);
		cl.clear();
		cl.push_back(fromRepl);
		cl.push_back(-fromVar);
		cl.push_back(-assump);
		// notAttackedRepl
		solver->addClause(cl);
		cl.clear();
		cl.push_back(-notAttackedRepl);
		cl.push_back(-fromVar-varMap.intVars().size());
		solver->addClause(cl);
		cl.clear();
		cl.push_back(-notAttackedRepl);
		cl.push_back(assump);
		solver->addClause(cl);
		cl.clear();
		cl.push_back(notAttackedRepl);
		cl.push_back(fromVar+varMap.intVars().size());
		cl.push_back(-assump);
		solver->addClause(cl);
	}
}


std::vector<int> SatEncodingHelper::dynAssumps(int step) {
	std::vector<int> assumps;
	for(int i=0; i<=step; ++i) {
		auto dynAttack = this->attacks.getDynAttacks()[(unsigned)i];
		int assump = 0;
		for(unsigned int j=0; j<this->dynVars.size(); ++j) {
			if(std::get<0>(this->dynVars[j]) == std::get<1>(dynAttack) && std::get<1>(this->dynVars[j]) == std::get<2>(dynAttack)) {
				auto assumpVar = std::get<4>(this->dynVars[j]);
				assump = std::get<0>(dynAttack) ? assumpVar : -assumpVar;
				break;
			}
		}
		bool found = false;
		for(unsigned int j=0; j<assumps.size(); ++j) {
			if(assumps[j] == assump) {
				//std::cerr << "assump already in" << std::endl;
				found = true;
				break;
			} else if(assumps[j] == -assump) {
				//std::cerr << "replacing assump " << assumps[j] << " by " << assump << std::endl;
				assumps[j] = assump;
				found = true;
				break;
			}
		}
		if(!found) assumps.push_back(assump);
		//if(!found) std::cerr << "assuming (before step) " << assumps[assumps.size()-1] << std::endl;
	}
	for(unsigned int i=step+1; i<this->attacks.getDynAttacks().size(); ++i) {
		auto dynAttack = this->attacks.getDynAttacks()[i];
		int assump = 0;
		for(unsigned int j=0; j<this->dynVars.size(); ++j) {
			if(std::get<0>(this->dynVars[j]) == std::get<1>(dynAttack) && std::get<1>(this->dynVars[j]) == std::get<2>(dynAttack)) {
				assump = std::get<4>(this->dynVars[j]);
				break;
			}
		}
		bool found = false;
		for(unsigned int j=0; j<assumps.size(); ++j) {
			if(assumps[j] == assump || assumps[j] == -assump) {
				found = true;
				break;
			}
		}
		if(!found) assumps.push_back(std::get<3>(dynAttack) ? assump : -assump);
		//if(!found) std::cerr << "assuming (after step) " << assumps[assumps.size()-1] << std::endl;
	}
	/* std::cerr << "assumps contains:";
	for(unsigned int i=0; i<assumps.size(); ++i) {
		std::cerr << " " << assumps[i];
	}
	std::cerr << std::endl; */
	return assumps;
}


SatEncodingHelper::~SatEncodingHelper() {}

