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
	while(candidates.size() > 0) {
		unsigned int step = 0;
		for(unsigned int i=0; i<candidates.size(); ++i) {
			int var = candidates[i];
			if(vm.isSelfAttacking(var)) {
				defeated[var-1] = true;
				candidates[i--] = candidates.back();
				candidates.pop_back();
				++step;
				continue;
			}
			bool argInExt = true;
			bool argDefeated = false;
			std::vector<int> attacksTo = *attacks.getAttacksTo(var);
			for(unsigned int j=0; j<attacksTo.size(); ++j) {
				int attacker = attacksTo[j];
				if(defeated[attacker-1]) continue;
				if(inExt[attacker-1]) {
					argDefeated = true;
					argInExt = false;
					break;
				}
				argInExt = false;
			}
			if(argInExt) {
				grExt.push_back(var);
				inExt[var-1] = true;
				candidates[i--] = candidates.back();
				candidates.pop_back();
				++step;
				continue;
			}
			if(argDefeated) {
				defeated[var-1] = true;
				candidates[i--] = candidates.back();
				candidates.pop_back();
				++step;
				continue;
			}
		}
		if(step == 0) break;
	}
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

