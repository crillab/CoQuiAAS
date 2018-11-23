#include "ICCMA17SolverOutputFormatter.h"

using namespace CoQuiAAS;

ICCMA17SolverOutputFormatter::ICCMA17SolverOutputFormatter(VarMap &varMap) : SolverOutputFormatter(), vmap(varMap) {
    // nothing to do here
}

std::string ICCMA17SolverOutputFormatter::formatArgAcceptance(bool status) {
    return acceptance_status_str(status);
}

std::string ICCMA17SolverOutputFormatter::formatNoExt() {
    return NO_STR;
}

std::string ICCMA17SolverOutputFormatter::formatSingleExtension(std::vector<bool>& model) {
    return argArray(model, this->vmap);
}

std::string ICCMA17SolverOutputFormatter::formatEveryExtension(std::vector<std::vector<bool>>& models) {
    std::vector<std::string> exts;
    for(unsigned int i=0; i<models.size(); ++i) {
        exts.push_back(formatSingleExtension(models[i]));
    }
    return formatSequenceOfExtensions(exts);
}

std::string ICCMA17SolverOutputFormatter::formatSequenceOfExtensions(std::vector<std::string> exts) {
    std::string res;
    bool first = true;
    res = "[";
    for(unsigned int i=0; i<exts.size(); ++i) {
        if(first) first=false; else res = res+",";
        res = res + exts[i];
    }
    res = res+"]";
    return res;
}

std::string ICCMA17SolverOutputFormatter::formatSingleExtension(std::vector<int>& lits) {
    return argArray(lits, this->vmap);
}

std::string ICCMA17SolverOutputFormatter::formatEveryExtension(std::vector<std::vector<int>>& lits) {
    std::vector<std::string> exts;
    for(unsigned int i=0; i<lits.size(); ++i) {
        exts.push_back(formatSingleExtension(lits[i]));
    }
    return formatSequenceOfExtensions(exts);
}

std::string ICCMA17SolverOutputFormatter::formatD3(std::string grExts, std::string stExts, std::string prExts) {
    return grExts+","+stExts+","+prExts;
}

ICCMA17SolverOutputFormatter::~ICCMA17SolverOutputFormatter() {
    // nothing to do here
}