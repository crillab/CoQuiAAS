#include "ICCMA19SolverOutputFormatter.h"

using namespace CoQuiAAS;

ICCMA19SolverOutputFormatter::ICCMA19SolverOutputFormatter(VarMap &varMap, void (*displayFct)(std::string)) : SolverOutputFormatter(displayFct), vmap(varMap) {
    // nothing to do here
}

void ICCMA19SolverOutputFormatter::writeArgAcceptance(bool status) {
    this->displayFct(acceptance_status_str(status)+"\n");
}

void ICCMA19SolverOutputFormatter::writeNoExt() {
    this->displayFct(NO_STR+"\n");
}

void ICCMA19SolverOutputFormatter::writeSingleExtension(std::vector<bool>& model) {
    this->displayFct(argArray(model, this->vmap)+"\n");
}

std::string ICCMA19SolverOutputFormatter::formatSequenceOfExtensions(std::vector<std::string> exts) {
    if(exts.size() == 0) return "[]";
    std::string res;
    res = "[";
    for(unsigned int i=0; i<exts.size(); ++i) {
        res = res + "\n" + exts[i];
    }
    res = res+"\n]";
    return res;
}

void ICCMA19SolverOutputFormatter::writeSingleExtension(std::vector<int>& lits) {
    this->displayFct(argArray(lits, this->vmap)+"\n");
}

void ICCMA19SolverOutputFormatter::writeExtensionListBegin() {
    this->displayFct("[");
}

void ICCMA19SolverOutputFormatter::writeExtensionListElmt(std::vector<bool>& model, bool isFirst) {
    this->displayFct("\n"+argArray(model, this->vmap));
}

void ICCMA19SolverOutputFormatter::writeExtensionListElmt(std::vector<int>& lits, bool isFirst) {
    this->displayFct("\n"+argArray(lits, this->vmap));
}

void ICCMA19SolverOutputFormatter::writeExtensionListEnd() {
    this->displayFct("\n]\n");
}

void ICCMA19SolverOutputFormatter::writeD3Begin() {
    // nothing to do here
}

void ICCMA19SolverOutputFormatter::writeD3GrExts(std::vector<int>& ext) {
    writeExtensionListBegin();
    writeExtensionListElmt(ext, true);
    writeExtensionListEnd();
    this->displayFct("\n");
}

void ICCMA19SolverOutputFormatter::writeD3StExts(std::vector<std::vector<int> >& exts) {
    writeExtensionListBegin();
    for(unsigned int i=0; i<exts.size(); ++i) {
        writeExtensionListElmt(exts[i], i==0);
    }
    writeExtensionListEnd();
    this->displayFct("\n");
}

void ICCMA19SolverOutputFormatter::writeD3PrExts(std::vector<std::vector<int> >& exts) {
    writeExtensionListBegin();
    for(unsigned int i=0; i<exts.size(); ++i) {
        writeExtensionListElmt(exts[i], i==0);
    }
    writeExtensionListEnd();
}

void ICCMA19SolverOutputFormatter::writeD3End() {
    // nothing to do here
}

void ICCMA19SolverOutputFormatter::writeDynListBegin(TaskType task) {
    this->displayFct("[");
    if(task == TASK_ONE_EXT || task == TASK_ALL_EXTS) this->displayFct("\n");
}

void ICCMA19SolverOutputFormatter::writeDynListElmtSep(TaskType task) {
    if(task == TASK_ONE_EXT || task == TASK_ALL_EXTS) this->displayFct("\n");
    else this->displayFct(",");
}

void ICCMA19SolverOutputFormatter::writeDynListEnd(TaskType task) {
    if(task == TASK_ONE_EXT || task == TASK_ALL_EXTS) this->displayFct("\n");
    this->displayFct("]\n");
}

ICCMA19SolverOutputFormatter::~ICCMA19SolverOutputFormatter() {
    // nothing to do here
}