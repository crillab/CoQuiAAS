#include "ICCMA19SolverOutputFormatter.h"

using namespace CoQuiAAS;

ICCMA19SolverOutputFormatter::ICCMA19SolverOutputFormatter(VarMap &varMap) : SolverOutputFormatter(), vmap(varMap) {
    // nothing to do here
}

std::string ICCMA19SolverOutputFormatter::formatArgAcceptance(bool status) {
    return acceptance_status_str(status);
}

std::string ICCMA19SolverOutputFormatter::formatNoExt() {
    return NO_STR;
}

std::string ICCMA19SolverOutputFormatter::formatSingleExtension(std::vector<bool>& model) {
    return argArray(model, this->vmap);
}

std::string ICCMA19SolverOutputFormatter::formatEveryExtension(std::vector<std::vector<bool>>& models) {
    std::vector<std::string> exts;
    for(unsigned int i=0; i<models.size(); ++i) {
        exts.push_back(formatSingleExtension(models[i]));
    }
    return formatSequenceOfExtensions(exts);
}

std::string ICCMA19SolverOutputFormatter::formatSequenceOfExtensions(std::vector<std::string> exts) {
    if(exts.size() == 0) return "[]";
    std::string res;
    bool first = true;
    res = "[";
    for(unsigned int i=0; i<exts.size(); ++i) {
        if(first) first=false; else res = res+"\n";
        res = res + exts[i];
    }
    res = res+"\n]";
    return res;
}

std::string ICCMA19SolverOutputFormatter::formatSingleExtension(std::vector<int>& lits) {
    return argArray(lits, this->vmap);
}

std::string ICCMA19SolverOutputFormatter::formatEveryExtension(std::vector<std::vector<int>>& lits) {
    std::vector<std::string> exts;
    for(unsigned int i=0; i<lits.size(); ++i) {
        exts.push_back(formatSingleExtension(lits[i]));
    }
    return formatSequenceOfExtensions(exts);
}

std::string ICCMA19SolverOutputFormatter::formatD3(std::string grExts, std::string stExts, std::string prExts) {
    return grExts+"\n"+stExts+"\n"+prExts;
}

ICCMA19SolverOutputFormatter::~ICCMA19SolverOutputFormatter() {
    // nothing to do here
}