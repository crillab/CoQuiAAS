#include "ICCMA17SolverOutputFormatter.h"

using namespace CoQuiAAS;

ICCMA17SolverOutputFormatter::ICCMA17SolverOutputFormatter(VarMap &varMap, void (*displayFct)(std::string)) : SolverOutputFormatter(displayFct), vmap(varMap) {
    // nothing to do here
}

void ICCMA17SolverOutputFormatter::writeArgAcceptance(bool status) {
    this->displayFct(acceptance_status_str(status)+"\n");
}

void ICCMA17SolverOutputFormatter::writeNoExt() {
    this->displayFct(NO_STR+"\n");
}

void ICCMA17SolverOutputFormatter::writeSingleExtension(std::vector<bool>& model) {
    this->displayFct(argArray(model, this->vmap)+"\n");
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

void ICCMA17SolverOutputFormatter::writeSingleExtension(std::vector<int>& lits) {
    this->displayFct(argArray(lits, this->vmap)+"\n");
}

void ICCMA17SolverOutputFormatter::writeExtensionListBegin() {
    this->displayFct("[");
}

void ICCMA17SolverOutputFormatter::writeExtensionListElmt(std::vector<bool>& model, bool isFirst) {
    if(!isFirst) this->displayFct(",");
    this->displayFct(argArray(model, this->vmap));
}

void ICCMA17SolverOutputFormatter::writeExtensionListElmt(std::vector<int>& lits, bool isFirst) {
    if(!isFirst) this->displayFct(",");
    this->displayFct(argArray(lits, this->vmap));
}

void ICCMA17SolverOutputFormatter::writeExtensionListEnd() {
    this->displayFct("]\n");
}

void ICCMA17SolverOutputFormatter::writeD3Begin() {
    // nothing to do here
}

void ICCMA17SolverOutputFormatter::writeD3GrExts(std::vector<int>& ext) {
    writeExtensionListBegin();
    writeExtensionListElmt(ext, true);
    writeExtensionListEnd();
    this->displayFct(",");
}

void ICCMA17SolverOutputFormatter::writeD3StExts(std::vector<std::vector<int> >& exts) {
    writeExtensionListBegin();
    for(unsigned int i=0; i<exts.size(); ++i) {
        writeExtensionListElmt(exts[i], i==0);
    }
    writeExtensionListEnd();
    this->displayFct(",");
}

void ICCMA17SolverOutputFormatter::writeD3PrExts(std::vector<std::vector<int> >& exts) {
    writeExtensionListBegin();
    for(unsigned int i=0; i<exts.size(); ++i) {
        writeExtensionListElmt(exts[i], i==0);
    }
    writeExtensionListEnd();
}

void ICCMA17SolverOutputFormatter::writeDynListBegin(TaskType task) {
    this->displayFct("[");
}

void ICCMA17SolverOutputFormatter::writeDynListElmtSep(TaskType task) {
    this->displayFct(",");
}

void ICCMA17SolverOutputFormatter::writeDynListEnd(TaskType task) {
    this->displayFct("]\n");
}

void ICCMA17SolverOutputFormatter::writeD3End() {
    // nothing to do here
}

ICCMA17SolverOutputFormatter::~ICCMA17SolverOutputFormatter() {
    // nothing to do here
}