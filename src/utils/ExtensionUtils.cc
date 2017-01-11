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
	std::vector<int> allVars = *attacks.getVarMap().intVars();
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
				defeated[var] = true;
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
				if(defeated[attacker]) continue;
				if(inExt[attacker]) {
					argDefeated = true;
					argInExt = false;
					break;
				}
				argInExt = false;
				break;
			}
			if(argInExt) {
				grExt.push_back(var);
				inExt[var] = true;
				candidates[i--] = candidates.back();
				candidates.pop_back();
				++step;
				continue;
			}
			if(argDefeated) {
				defeated[var] = true;
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


ExtensionUtils::~ExtensionUtils() {}

