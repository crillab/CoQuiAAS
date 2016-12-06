/*
 * ExternalCoMssSolver.cc
 *
 *  Created on: 21 juil. 2016
 *      Author: lonca
 */


#include "ExternalMssSolver.h"


using namespace CoQuiAAS;


ExternalMssSolver::ExternalMssSolver(std::string command) : ExternalSatSolver(command) {
	this->nSoftCstrs = 0;
	this->computingModel = false;
}


void ExternalMssSolver::addSoftClause(std::vector<int> &clause) {
	for(std::vector<int>::iterator itCl = clause.begin(); itCl != clause.end(); ++itCl) {
		this->dimacsSoftCstrs << *itCl << " ";
	}
	this->dimacsSoftCstrs << "0\n";
	++nSoftCstrs;
}


void ExternalMssSolver::computeMss() {
	std::vector<int> assumps;
	computeMss(assumps);
}


void ExternalMssSolver::computeMss(std::vector<int> &assumps) {
	computeMss(assumps, true);
}


void ExternalMssSolver::computeMss(std::vector<int> &assumps, bool clearMssVec) {
	if(clearMssVec) this->mss.clear();
	char *tmpname = strdup("/tmp/tmp_CoQuiASS_ext_XXXXXX");
	if(-1==mkstemp(tmpname)) {
		perror("ExternalSatBasedSolver::hasAModel::mkstemp");
		exit(2);
	}
	std::ofstream f(tmpname);
	int hardCstrWeight = (this->nSoftCstrs+1);
	f << "p wcnf " << this->nVars << " " << (unsigned int)(this->nSoftCstrs+this->nCstrs+assumps.size()) << " " << hardCstrWeight <<std::endl;
	std::string line;
	std::istringstream softCstrsStream(this->dimacsSoftCstrs.str());
	while (std::getline(softCstrsStream, line)) {
		f << "1 " << line << std::endl;
	}
	std::istringstream hardCstrsStream(this->dimacsCstrs.str());
	while (std::getline(hardCstrsStream, line)) {
		f << hardCstrWeight << " " << line << std::endl;
	}
	for(std::vector<int>::iterator it = assumps.begin(); it != assumps.end(); ++it) {
		f << hardCstrWeight << " " << *it << " 0" << std::endl;
	}
	f.close();
	launchExternalSolver(std::string(tmpname));
	unlink(tmpname);
	free(tmpname);
}


void ExternalMssSolver::handleForkAncestor(int pipe[]) {
	if(computingModel) {
		ExternalSatSolver::handleForkAncestor(pipe);
		return;
	}
	wait(NULL);
	close(pipe[1]);
	FILE *childOutFile = fdopen(pipe[0],"r");
	char buffer[EXTERNAL_SAT_BUFFER_SIZE];
	while(fgets(buffer, EXTERNAL_SAT_BUFFER_SIZE, childOutFile)) {
		if(!strncmp(buffer, "s HARD PART UNSATISFIABLE", 25)) {
			return;
		}
		if(!strncmp(buffer, "v ", 2)) {
			extractMss(buffer, childOutFile);
		}
	}
	fclose(childOutFile);
	close(pipe[0]);
}


void ExternalMssSolver::extractMss(char buffer[], FILE *childOutFile) {
	char *pc = buffer+2;
	int nb = 0;
	bool readingNb = false;
	std::vector<int> tmpMss;
	for(int i=1; i<=nSoftCstrs; ++i) tmpMss.push_back(i);
	for(; *pc != '\n'; ++pc) {
		if(!*pc) {
			fgets(buffer, EXTERNAL_COMSS_BUFFER_SIZE, childOutFile);
			pc = buffer;
		}
		if((*pc == ' ' || *pc == '\t') && readingNb) {
			if(!nb) break;
			tmpMss[nb-1] = -1;
			nb = 0;
			readingNb = false;
		} else if(*pc >= '0' && *pc <= '9') {
			nb = 10*nb + (*pc - '0');
			readingNb = true;
		}
	}
	std::vector<int> newMss;
	for(unsigned int i=0; i<tmpMss.size(); ++i) {
		if(tmpMss[i] > 0) {
			newMss.push_back(tmpMss[i]);
		}
	}
	this->mss.push_back(newMss);
}


void ExternalMssSolver::computeAllMss() {
	std::vector<int> assumps;
	computeAllMss(assumps);
}


void ExternalMssSolver::computeAllMss(std::vector<int> &assumps) {
	this->mss.clear();
	unsigned int nbMss = 0;
	std::vector<int> blockingSelectors;
	for(;;) {
		computeMss(assumps, false);
		if(this->mss.size() > nbMss) {
			std::vector<int> found = this->mss[this->mss.size()-1];
			std::vector<int> opposite;
			for(int i=1; i<=nSoftCstrs; ++i) opposite.push_back(i);
			for(unsigned int i=0; i<found.size(); ++i) {
				opposite[found[i]-1] = -1;
			}
			std::vector<int> blocking;
			for(unsigned int i=0; i<opposite.size(); ++i) {
				if(opposite[i] > 0) blocking.push_back(opposite[i]);
			}
			int sel = addSelectedClause(blocking);
			blockingSelectors.push_back(sel);
			assumps.push_back(sel);
			++nbMss;
		} else {
			break;
		}
	}
	for(int i=0; i<(int) blockingSelectors.size(); ++i) {
			std::vector<int> cl;
			cl.push_back(-blockingSelectors[i]);
			addClause(cl);
		}
}


bool ExternalMssSolver::hasAMss() {
	return this->mss.size() > 0;
}


std::vector<int>& ExternalMssSolver::getMss() {
	return this->mss[0];
}


std::vector<std::vector<int> >& ExternalMssSolver::getAllMss() {
	return this->mss;
}


ExternalMssSolver::~ExternalMssSolver() {}


void ExternalMssSolver::addVariables(int nVars) {
	ExternalSatSolver::addVariables(nVars);
}


bool ExternalMssSolver::addClause(std::vector<int> &clause) {
	return ExternalSatSolver::addClause(clause);
}

int ExternalMssSolver::addSelectedClause(std::vector<int> &clause) {
	return ExternalSatSolver::addSelectedClause(clause);
}


std::vector<int>& ExternalMssSolver::propagatedAtDecisionLvlZero() {
	return ExternalSatSolver::propagatedAtDecisionLvlZero();
}


bool ExternalMssSolver::isPropagatedAtDecisionLvlZero(int lit) {
	return ExternalSatSolver::isPropagatedAtDecisionLvlZero(lit);
}


void ExternalMssSolver::computeModel() {
	this->computingModel = true;
	ExternalSatSolver::computeModel();
	this->computingModel = false;
}


void ExternalMssSolver::computeModel(std::vector<int> &assumps) {
	this->computingModel = true;
	ExternalSatSolver::computeModel(assumps);
	this->computingModel = false;
}


void ExternalMssSolver::computeAllModels() {
	this->computingModel = true;
	ExternalSatSolver::computeAllModels();
	this->computingModel = false;
}


void ExternalMssSolver::computeAllModels(std::vector<int> &assumps) {
	this->computingModel = true;
	ExternalSatSolver::computeAllModels(assumps);
	this->computingModel = false;
}


bool ExternalMssSolver::hasAModel() {
	return ExternalSatSolver::hasAModel();
}


std::vector<bool>& ExternalMssSolver::getModel() {
	return ExternalSatSolver::getModel();
}


std::vector<std::vector<bool> >& ExternalMssSolver::getModels() {
	return ExternalSatSolver::getModels();
}
