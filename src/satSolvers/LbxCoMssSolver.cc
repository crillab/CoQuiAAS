/*
 * LbxCoMssSolver.cc
 *
 *  Created on: 14 déc. 2016
 *      Author: lonca
 */

#include "LbxCoMssSolver.h"


using namespace CoQuiAAS;


LbxCoMssSolver::LbxCoMssSolver(std::string lbxPath) {
	this->lbxPath = lbxPath;
	this->nSoftCstrs = 0;
	this->nVars = 0;
	this->nCstrs = 0;
}


LbxCoMssSolver::~LbxCoMssSolver() {}


void LbxCoMssSolver::addSoftClause(std::vector<int> &clause) {
	for(std::vector<int>::iterator itCl = clause.begin(); itCl != clause.end(); ++itCl) {
		this->dimacsSoftCstrs << *itCl << " ";
	}
	this->dimacsSoftCstrs << "0\n";
	++nSoftCstrs;
}


void LbxCoMssSolver::clearMss() {
	this->mss.clear();
	this->models.clear();
}


bool LbxCoMssSolver::computeMss() {
	std::vector<int> assumps;
	return computeMss(assumps);
}


bool LbxCoMssSolver::computeMss(std::vector<int> &assumps) {
	std::string instance = writeInstance(assumps, false);
	int ret = launchExternalSolver(instance, false);
	unlink(instance.c_str());
	return ret;
}


void LbxCoMssSolver::computeAllMss() {
	std::vector<int> assumps;
	computeAllMss(assumps);
}


void LbxCoMssSolver::computeAllMss(std::vector<int> &assumps) {
	std::string instance = writeInstance(assumps, false);
	launchExternalSolver(instance, true);
	unlink(instance.c_str());
}


bool LbxCoMssSolver::hasAMss() {
	return this->mss.size() > 0;
}


std::vector<int>& LbxCoMssSolver::getMss() {
	return this->mss[this->mss.size()-1];
}


std::vector<std::vector<int> >& LbxCoMssSolver::getAllMss() {
	return this->mss;
}

std::string LbxCoMssSolver::writeInstance(std::vector<int> assumps, bool onlyHardClauses) {
	std::string tmpname("/tmp/tmp_CoQuiASS_ext_XXXXXX");
	if(-1==mkstemp((char *) tmpname.c_str())) {
		perror("ExternalSatBasedSolver::hasAModel::mkstemp");
		exit(2);
	}
	std::ofstream f(tmpname.c_str());
	int hardCstrWeight = (this->nSoftCstrs+1);
	unsigned int nClauses = onlyHardClauses ? this->nCstrs+assumps.size() : this->nSoftCstrs+this->nCstrs+assumps.size();
	f << "p wcnf " << this->nVars << " " << nClauses << " " << hardCstrWeight <<std::endl;
	std::string line;
	if(!onlyHardClauses) {
		std::istringstream softCstrsStream(this->dimacsSoftCstrs.str());
		while (std::getline(softCstrsStream, line)) {
			f << "1 " << line << std::endl;
		}
	}
	std::istringstream hardCstrsStream(this->dimacsCstrs.str());
	while (std::getline(hardCstrsStream, line)) {
		f << hardCstrWeight << " " << line << std::endl;
	}
	for(std::vector<int>::iterator it = assumps.begin(); it != assumps.end(); ++it) {
		f << hardCstrWeight << " " << *it << " 0" << std::endl;
	}
	f.close();
	return tmpname;
}


bool LbxCoMssSolver::launchExternalSolver(std::string instanceFile, bool allModels) {
	pid_t pid;
	int pfds[2];

	if(-1 == pipe(pfds)) {perror("CoQuiAAS");exit(1);}
	if(-1 == (pid = fork())) {perror("CoQuiAAS");exit(1);}
	if(pid > 0) {
		return handleForkAncestor(pfds);
	} else {
		handleForkChild(instanceFile, allModels, pfds);
		return true; // will never be returned since exec() is called in fork child
	}
}


void LbxCoMssSolver::handleForkChild(std::string instanceFile, bool allModels, int pfds[]) {
	close(1);
	if(-1==dup(pfds[1])) {
		perror("launchExternalSolver::dup");
		exit(2);
	}
	close(pfds[0]);
	if(allModels) {
		execl(this->lbxPath.c_str(), this->lbxPath.c_str(), "-wm", instanceFile.c_str(), NULL);
	} else {
		execl(this->lbxPath.c_str(), this->lbxPath.c_str(), "-wm", "-num", "1", instanceFile.c_str(), NULL);
	}
	perror("CoQuiAAS");
	exit(1);
}


bool LbxCoMssSolver::handleForkAncestor(int pipe[]) {
	wait(NULL);
	close(pipe[1]);
	bool ret = false;
	FILE *childOutFile = fdopen(pipe[0], "r");
	char buffer[BUF_READ_SIZE];
	while(fgets(buffer, BUF_READ_SIZE, childOutFile)) {
		if(!strncmp(buffer, "c MCS: ", 7)) {
			this->mss.push_back(extractCoMss(buffer+7));
			ret = true;
			continue;
		}
		if(!strncmp(buffer, "c model: ", 9)) {
			this->models.push_back(extractModel(buffer+9));
			continue;
		}
	}
	fclose(childOutFile);
	close(pipe[0]);
	return ret;
}


std::vector<int> LbxCoMssSolver::extractCoMss(char *line) {
	std::vector<int> tmpMss;
	for(int i=1; i<=nSoftCstrs; ++i) tmpMss.push_back(i);
	std::vector<int> mcs = readIntVector(line);
	for(unsigned int i=0; i<mcs.size(); ++i) tmpMss[mcs[i]-1] = -1;
	std::vector<int> newMss;
	for(unsigned int i=0; i<tmpMss.size(); ++i) {
		if(tmpMss[i] > 0) {
			newMss.push_back(tmpMss[i]);
		}
	}
	return newMss;
}


std::vector<int> LbxCoMssSolver::readIntVector(char *line) {
	std::vector<int> vec;
	char *pc = line;
	bool readingNb = false;
	int nb = 0;
	bool minus = false;
	for(; *pc && (*pc != '\n'); ++pc) {
		if((*pc == ' ' || *pc == '\t') && readingNb) {
			if(!nb) break;
			vec.push_back(minus ? -nb : nb);
			nb = 0;
			minus = false;
			readingNb = false;
		} else if (*pc == '-' && !readingNb) {
			minus = true;
		} else if(*pc >= '0' && *pc <= '9') {
			nb = 10*nb + (*pc - '0');
			readingNb = true;
		}
	}
	return vec;
}


std::vector<bool> LbxCoMssSolver::extractModel(char *line) {
	std::vector<int> intModel = readIntVector(line);
	std::vector<bool> boolModel;
	for(unsigned int i=0; i<intModel.size(); ++i) {
		int val = intModel[i];
		if(!val) break;
		boolModel.push_back(val > 0);
	}
	return boolModel;
}


void LbxCoMssSolver::addVariables(int nVars) {
	this->nVars += nVars;
}


bool LbxCoMssSolver::addClause(std::vector<int> &clause) {
	for(std::vector<int>::iterator itCl = clause.begin(); itCl != clause.end(); ++itCl) {
		dimacsCstrs << *itCl << " ";
	}
	dimacsCstrs << "0\n";
	++nCstrs;
	return true;
}


int LbxCoMssSolver::addSelectedClause(std::vector<int> &clause) {
	addVariables(1);
	int selector = this->nVars;
	clause.push_back(-selector);
	addClause(clause);
	return selector;
}


std::vector<int>& LbxCoMssSolver::propagatedAtDecisionLvlZero() {
	std::cerr << "operation unavailable for external solver" << std::endl;
	exit(1);
}


bool LbxCoMssSolver::isPropagatedAtDecisionLvlZero(int lit) {
	std::cerr << "operation unavailable for external solver" << std::endl;
	exit(1);
}


bool LbxCoMssSolver::computeModel() {
	std::vector<int> assumps;
	return computeModel(assumps);
}


bool LbxCoMssSolver::computeModel(std::vector<int> &assumps) {
	std::string instance = writeInstance(assumps, true);
	int ret = launchExternalSolver(instance, false);
	unlink(instance.c_str());
	return ret;
}


void LbxCoMssSolver::computeAllModels() {
	std::vector<int> assumps;
	return computeAllModels(assumps);
}


void LbxCoMssSolver::computeAllModels(std::vector<int> &assumps) {
	std::string instance = writeInstance(assumps, true);
	launchExternalSolver(instance, false);
	unlink(instance.c_str());
}


bool LbxCoMssSolver::hasAModel() {
	return this->models.size() > 0;
}


std::vector<bool>& LbxCoMssSolver::getModel() {
	return this->models[this->models.size()-1];
}


std::vector<std::vector<bool> >& LbxCoMssSolver::getModels() {
	return this->models;
}

