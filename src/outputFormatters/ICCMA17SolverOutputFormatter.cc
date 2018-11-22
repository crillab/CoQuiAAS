#include "ICCMA17SolverOutputFormatter.h"

using namespace CoQuiAAS;

ICCMA17SolverOutputFormatter::ICCMA17SolverOutputFormatter(VarMap &varMap) : SolverOutputFormatter(), vmap(varMap) {
    // nothing to do here
}

std::string ICCMA17SolverOutputFormatter::formatArgAcceptance(bool status) {
    return status ? "YES" : "NO";
}

std::string ICCMA17SolverOutputFormatter::formatNoExt() {
    return "NO";
}

std::string ICCMA17SolverOutputFormatter::formatSingleExtension(std::vector<bool>& model) {
    std::string res;

    bool noneFoundYet = true;
    res = "[";
    int nVars = this->vmap.nVars();
    for (int i = 0; i < nVars; i++) {
        if (model[i]) {
            if(noneFoundYet) noneFoundYet=false; else res=res + ",";
            res = res + this->vmap.getName(i+1);
        }
    }
    res = res + "]";
    return res;
}

std::string ICCMA17SolverOutputFormatter::formatEveryExtension(std::vector<std::vector<bool>>& model) {
    std::string res;
    bool first = true;
    res = "[";
    for(int i=0; i<model.size(); ++i) {
        if(first) first=false; else res = res+",";
        res = res + this->formatSingleExtension(model[i]);
    }
    res = res+"]";
    return res;
}

std::string ICCMA17SolverOutputFormatter::formatSingleExtension(std::vector<int>& lits) {
    std::string answer = "[";
	int nPropagated = (signed) lits.size();
	int nArgs = this->vmap.nVars();
	int nPropagatedArgs = 0;
	for(int i=0; i<nPropagated; ++i) {
		if(lits[i] <= nArgs) {
			if(nPropagatedArgs > 0) answer = answer+",";
			++nPropagatedArgs;
			answer = answer+this->vmap.getName(lits[i]);
		}
	}
	return answer + "]";
}

std::string ICCMA17SolverOutputFormatter::formatEveryExtension(std::vector<std::vector<int>>& lits) {
    std::string res;
    bool first = true;
    res = "[";
    for(int i=0; i<lits.size(); ++i) {
        if(first) first=false; else res = res+",";
        res = res + this->formatSingleExtension(lits[i]);
    }
    res = res+"]";
    return res;
}

std::string ICCMA17SolverOutputFormatter::formatD3(std::string grExts, std::string stExts, std::string prExts) {
    return grExts+","+stExts+","+prExts;
}

ICCMA17SolverOutputFormatter::~ICCMA17SolverOutputFormatter() {
    // nothing to do here
}