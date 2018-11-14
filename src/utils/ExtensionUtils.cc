/*
 * ExtensionUtils.cc
 *
 *  Created on: 11 janv. 2017
 *      Author: lonca
 */

#include "ExtensionUtils.h"

using namespace CoQuiAAS;


ExtensionUtils::ExtensionUtils(Attacks &attacks) : attacks(attacks) {}


std::vector<int> ExtensionUtils::groundedExtension() {
	VarMap& vm = attacks.getVarMap();
	std::vector<int> grExt;
	std::vector<int>& allVars = attacks.getVarMap().intVars();
	std::vector<bool> inExt;
	for(unsigned int i=0; i<allVars.size(); ++i) inExt.push_back(false);
	std::vector<bool> defeated;
	for(unsigned int i=0; i<allVars.size(); ++i) defeated.push_back(false);
	std::vector<int> candidates(allVars);
	bool didSomething = false;
	do {
		didSomething = false;
		for(unsigned int i=0; i<candidates.size(); ++i) {
			int var = candidates[i];
			std::vector<int> attacksTo = *attacks.getAttacksTo(var);
			bool argDefeated = false;
			bool argAttacked = false;
			for(unsigned int j=0; j<attacksTo.size(); ++j) {
				int attacker = attacksTo[j];
				if(defeated[attacker-1]) continue;
				argAttacked = true;
				if(inExt[attacker-1]) {
					defeated[var-1] = true;
					candidates[i--] = candidates.back();
					candidates.pop_back();
					argDefeated = true;
					didSomething = true;
					break;
				}
			}
			if(argDefeated) continue;
			if(!argAttacked) {
				grExt.push_back(var);
				inExt[var-1] = true;
				candidates[i--] = candidates.back();
				candidates.pop_back();
				didSomething = true;
			}
		}
	} while(didSomething);
	return grExt;
}


bool ExtensionUtils::isMaxRange(std::vector<int>& extension) {
	std::vector<int>& allVars = attacks.getVarMap().intVars();
	std::vector<bool> inExt;
	for(unsigned int i=0; i<allVars.size(); ++i) {
		inExt.push_back(false);
	}
	for(unsigned int i=0; i<extension.size(); ++i) {
		inExt[extension[i]-1] = true;
	}
	for(unsigned int i=0; i<inExt.size(); ++i) {
		if(inExt[i]) continue;
		std::vector<int> attackers = *attacks.getAttacksTo(i+1);
		bool attacked = false;
		for(unsigned int j=0; j<attackers.size(); ++j) {
			if(inExt[attackers[j]-1]) {
				attacked = true;
				break;
			}
		}
		if(!attacked) return false;
	}
	return true;
}


ExtensionUtils::~ExtensionUtils() {}

