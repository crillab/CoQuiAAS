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
	reserveDynVarsForCompleteSemantics();
	return ret;
}


void SatEncodingHelper::createAttackersDisjunctionVars(int startId) {
	/* printf("(disj) solver has %d vars\n", this->nbVars);
	printf("(disj) startId is %d\n", startId); */
	std::vector<std::string> vars = varMap.getNames();
	Minisat::vec<Minisat::Lit> binaryClause, naryClause;
	std::vector<int> binaryCl, completeCl;
	for(std::vector<std::string>::iterator itVars = vars.begin() ; itVars != vars.end(); ++itVars) {
		std::vector<std::string>& attacksToCurrentVar = this->attacks.getAttacksTo(*itVars);
		int var = varMap.getVar(*itVars);
		binaryCl.push_back(-var);
		binaryCl.push_back(-(var+startId-1));
		solver->addClause(binaryCl); // (-a \lor -Pa)
		binaryCl.clear();
		//std::cout << "added " << binaryCl[0] << " " << binaryCl[1] << std::endl;
		completeCl.push_back(-(var+startId-1));
		for(std::vector<std::string>::iterator itAttackers = attacksToCurrentVar.begin(); itAttackers != attacksToCurrentVar.end(); ++itAttackers) {
			int attacker = lookForDynAttackerReplacement(*itVars, *itAttackers);
			binaryCl.push_back(var+startId-1);
			binaryCl.push_back(-attacker);
			solver->addClause(binaryCl); // Pa \lor -b
			//std::cout << "added " << binaryCl[0] << " " << binaryCl[1] << std::endl;
			binaryCl.clear();
			completeCl.push_back(attacker);
		}
		/* std::cout << "added";
		for(unsigned int i=0; i<completeCl.size(); ++i) std::cout << " " << completeCl[i];
		std::cout << std::endl; */
		solver->addClause(completeCl); // -Pa \lor b1 \lor ... \lor bp
		completeCl.clear();
	}
}


void SatEncodingHelper::createCompleteEncodingConstraints(int startId) {
	//printf("(CO) solver has %d vars\n", this->nbVars);
	std::vector<int> binaryCl, completeCl;
	std::vector<std::string> vars = varMap.getNames();
	for(std::vector<std::string>::iterator itVars = vars.begin() ; itVars != vars.end(); ++itVars) {
		std::vector<std::string>& attacksToCurrentVar = attacks.getAttacksTo(*itVars);
		int var = varMap.getVar(*itVars);
		completeCl.push_back(var);
		for(std::vector<std::string>::iterator itAttackers = attacksToCurrentVar.begin(); itAttackers != attacksToCurrentVar.end(); ++itAttackers) {
			int attacker = varMap.getVar(*itAttackers);
			int attackerDisjOfAttacker = attacker+startId-1;
			for(unsigned int i=0; i<this->dynVars.size(); ++i) {
				if(std::get<0>(this->dynVars[i]) == *itAttackers && std::get<1>(this->dynVars[i]) == *itVars) {
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


/* void SatEncodingHelper::createCompleteEncodingConstraints(int startId) {
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
} */


void SatEncodingHelper::createConflictFreenessEncodingConstraints(int startId) {
	std::vector<int> binaryCl;
	std::vector<std::string> vars = varMap.getNames();
	for(std::vector<std::string>::iterator itVars = vars.begin() ; itVars != vars.end(); ++itVars) {
		std::vector<std::string>& attacksToCurrentVar = this->attacks.getAttacksTo(*itVars);
		for(std::vector<std::string>::iterator itAttackers = attacksToCurrentVar.begin(); itAttackers != attacksToCurrentVar.end(); ++itAttackers) {
			int attacker = lookForDynAttackerReplacement(*itVars, *itAttackers);
			binaryCl.push_back(-this->varMap.getVar(*itVars));
			binaryCl.push_back(-attacker); // -a \lor -b
			solver->addClause(binaryCl);
			binaryCl.clear();
		}
	}
}


int SatEncodingHelper::lookForDynAttackerReplacement(std::string attacked, std::string attacker) {
	for(unsigned int i=0; i<this->dynVars.size(); ++i) {
		if(std::get<0>(this->dynVars[i]) == attacker && std::get<1>(this->dynVars[i]) == attacked) {
			return std::get<2>(this->dynVars[i]);
		}
	}
	return this->varMap.getVar(attacker);
}


void SatEncodingHelper::createStableEncodingConstraints() {
	reserveDynVarsForStableSemantics();
	std::vector<int> binaryCl, completeCl;
	std::vector<std::string> vars = varMap.getNames();
	for(std::vector<std::string>::iterator itVars = vars.begin() ; itVars != vars.end(); ++itVars) {
		int var = this->varMap.getVar(*itVars);
		completeCl.push_back(var);
		std::vector<std::string>& attacksToCurrentVar = attacks.getAttacksTo(varMap.getName(var));
		for(std::vector<std::string>::iterator itAttackers = attacksToCurrentVar.begin(); itAttackers != attacksToCurrentVar.end(); ++itAttackers) {
			int attacker = lookForDynAttackerReplacement(*itVars, *itAttackers);
			binaryCl.push_back(-(var));
			if(var != attacker) binaryCl.push_back(-(attacker));
			solver->addClause(binaryCl); // -a \lor -b
			binaryCl.clear();
			if(var != attacker) completeCl.push_back(attacker);
		}
		solver->addClause(completeCl); // a \lor b1 \lor ... \lor bp
		completeCl.clear();
	}
}


void SatEncodingHelper::reserveDynVarsForCompleteSemantics() {
	reserveDynVars(true);
}


void SatEncodingHelper::reserveDynVars(bool reserveNotAttackedRepl) {
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
		std::string from = std::get<1>(dynAttacks[i]);
		std::string to = std::get<2>(dynAttacks[i]);
		int offset = reserveNotAttackedRepl ? 1 : 0;
		int fromRepl = reserveVars(2 + offset);
		int notAttackedRepl = reserveNotAttackedRepl ? fromRepl + 1 : 0;
		int assump = fromRepl + 1 + offset;
		this->dynVars.push_back(std::make_tuple(from, to, fromRepl, notAttackedRepl, assump));
		std::vector<int> cl;
		int fromVar = this->varMap.getVar(from);
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
		solver->addClause(cl);
		// notAttackedRepl
		if(reserveNotAttackedRepl) {
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
}


void SatEncodingHelper::reserveDynVarsForStableSemantics() {
	reserveDynVars(false);
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
				found = true;
				break;
			} else if(assumps[j] == -assump) {
				assumps[j] = assump;
				found = true;
				break;
			}
		}
		if(!found) assumps.push_back(assump);
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
	}
	return assumps;
}


SatEncodingHelper::~SatEncodingHelper() {}

