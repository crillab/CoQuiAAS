/*
 * ExternalMaxSatSolver.cc
 *
 *  Created on: 5 déc. 2016
 *      Author: lonca
 */


#include "ExternalMaxSatSolver.h"


using namespace CoQuiAAS;


ExternalMaxSatSolver::ExternalMaxSatSolver(std::string command) : ExternalSatSolver(command) {
	this->nSoftCstrs = 0;
	this->computingModel = false;
	this->optValue = -1;
	this->lastObjValue = -1;
}


void ExternalMaxSatSolver::addSoftClause(std::vector<int> &clause) {
	for(std::vector<int>::iterator itCl = clause.begin(); itCl != clause.end(); ++itCl) {
		this->dimacsSoftCstrs << *itCl << " ";
	}
	this->dimacsSoftCstrs << "0\n";
	++nSoftCstrs;
}


bool ExternalMaxSatSolver::computeMaxSat() {
	std::vector<int> assumps;
	return computeMaxSat(assumps);
}


bool ExternalMaxSatSolver::computeMaxSat(std::vector<int> &assumps) {
	return computeMaxSat(assumps, true);
}


bool ExternalMaxSatSolver::computeMaxSat(std::vector<int> &assumps, bool clearModelVec) {
	if(clearModelVec) this->models.clear();
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
	for(std::vector<int>::iterator it = assumps.begin(); it != assumps.end(); ++it)
		f << hardCstrWeight << " " << *it << " 0" << std::endl;
	f.close();
	int result = launchExternalSolver(std::string(tmpname));
	unlink(tmpname);
	free(tmpname);
	return result;
}


bool ExternalMaxSatSolver::handleForkAncestor(int pipe[]) {
	bool msFound = false;
	if(computingModel) {
		return ExternalSatSolver::handleForkAncestor(pipe);
	}
	wait(NULL);
	close(pipe[1]);
	FILE *childOutFile = fdopen(pipe[0],"r");
	char buffer[EXTERNAL_SAT_BUFFER_SIZE];
	while(fgets(buffer, EXTERNAL_SAT_BUFFER_SIZE, childOutFile)) {
		if(!strncmp(buffer, "s UNSATISFIABLE", 15)) {
			break;
		}
		if(!strncmp(buffer, "o ", 2)) {
			char *endptr;
			this->lastObjValue = (int) strtol(buffer+2, &endptr, 10);
			continue;
		}
		if(!strncmp(buffer, "v ", 2)) {
			if(this->optValue == -1) this->optValue = this->lastObjValue;
			if(this->lastObjValue > this->optValue) break;
			extractMaxSat(buffer, childOutFile);
			msFound = true;
			break;
		}
	}
	fclose(childOutFile);
	close(pipe[0]);
	return msFound;
}


void ExternalMaxSatSolver::extractMaxSat(char buffer[], FILE *childOutFile) {
	extractModel(buffer, childOutFile);
}


void ExternalMaxSatSolver::computeAllMaxSat() {
	std::vector<int> assumps;
	computeAllMaxSat(assumps);
}


void ExternalMaxSatSolver::computeAllMaxSat(std::vector<int> &assumps) {
	for(;;) {
		computeMaxSat(assumps, false);
		if(this->lastObjValue > this->optValue) break;
		int sel = addBlockingClause();
		blockingSelectors.push_back(sel);
	}
	for(int i=0; i<(int) blockingSelectors.size(); ++i) {
		std::vector<int> cl;
		cl.push_back(-blockingSelectors[i]);
		addClause(cl);
	}
}


std::vector<bool>& ExternalMaxSatSolver::getMaxSat() {
	return this->models[this->models.size()-1];
}


std::vector<std::vector<bool> >& ExternalMaxSatSolver::getAllMaxSat() {
	return this->models;
}


int ExternalMaxSatSolver::getOptValue() {
	return this->optValue;
}


ExternalMaxSatSolver::~ExternalMaxSatSolver() {}


void ExternalMaxSatSolver::addVariables(int nVars) {
	ExternalSatSolver::addVariables(nVars);
}

void ExternalMaxSatSolver::addVariables(int nVars, bool auxVars) {
	ExternalSatSolver::addVariables(nVars, auxVars);
}


bool ExternalMaxSatSolver::addClause(std::vector<int> &clause) {
	return ExternalSatSolver::addClause(clause);
}

int ExternalMaxSatSolver::addSelectedClause(std::vector<int> &clause) {
	return ExternalSatSolver::addSelectedClause(clause);
}


std::vector<int>& ExternalMaxSatSolver::propagatedAtDecisionLvlZero() {
	return ExternalSatSolver::propagatedAtDecisionLvlZero();
}


bool ExternalMaxSatSolver::isPropagatedAtDecisionLvlZero(int lit) {
	return ExternalSatSolver::isPropagatedAtDecisionLvlZero(lit);
}


bool ExternalMaxSatSolver::computeModel() {
	this->computingModel = true;
	bool result = ExternalSatSolver::computeModel();
	this->computingModel = false;
	return result;
}


bool ExternalMaxSatSolver::computeModel(std::vector<int> &assumps) {
	this->computingModel = true;
	bool result = ExternalSatSolver::computeModel(assumps);
	this->computingModel = false;
	return result;
}


void ExternalMaxSatSolver::computeAllModels(std::function<void(std::vector<bool>&)> callback) {
	this->computingModel = true;
	ExternalSatSolver::computeAllModels(callback);
	this->computingModel = false;
}


void ExternalMaxSatSolver::computeAllModels(std::function<void(std::vector<bool>&)> callback, std::vector<int> &assumps) {
	this->computingModel = true;
	ExternalSatSolver::computeAllModels(callback, assumps);
	this->computingModel = false;
}


bool ExternalMaxSatSolver::hasAModel() {
	return ExternalSatSolver::hasAModel();
}


std::vector<bool>& ExternalMaxSatSolver::getModel() {
	return ExternalSatSolver::getModel();
}


std::vector<std::vector<bool> >& ExternalMaxSatSolver::getModels() {
	return ExternalSatSolver::getModels();
}

void ExternalMaxSatSolver::resetModels() {
	this->models.clear();
}
