#include "SolverOutputFormatter.h"

using namespace CoQuiAAS;


std::string SolverOutputFormatter::acceptance_status_str(bool status) {
    return status ? YES_STR : NO_STR;
}

std::string SolverOutputFormatter::argArray(std::vector<bool>& model, VarMap& vmap) {
    std::string res;
    bool noneFoundYet = true;
    res = "[";
    int nVars = vmap.nVars();
    for (int i = 0; i < nVars; i++) {
        if (model[i]) {
            if(noneFoundYet) noneFoundYet=false; else res=res + ",";
            res = res + vmap.getName(i+1);
        }
    }
    res = res + "]";
    return res;
}

std::string SolverOutputFormatter::argArray(std::vector<int>& lits, VarMap& vmap) {
    std::string answer = "[";
	int nPropagated = (signed) lits.size();
	int nArgs = vmap.nVars();
	int nPropagatedArgs = 0;
	for(int i=0; i<nPropagated; ++i) {
		if(lits[i] <= nArgs) {
			if(nPropagatedArgs > 0) answer = answer+",";
			++nPropagatedArgs;
			answer = answer+vmap.getName(lits[i]);
		}
	}
	return answer + "]";
}