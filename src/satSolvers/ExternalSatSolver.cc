/*
 * ExternalSatSolver.cc
 *
 *  Created on: 21 juil. 2016
 *      Author: lonca
 */


#include "ExternalSatSolver.h"


ExternalSatSolver::ExternalSatSolver(std::string command) {
	this->nVars = 0;
	this->nCstrs = 0;
	this->command = command;
}


void ExternalSatSolver::addVariables(int nVars) {
	this->nVars += nVars;
}


bool ExternalSatSolver::addClause(std::vector<int> &clause) {
	for(std::vector<int>::iterator itCl = clause.begin(); itCl != clause.end(); ++itCl) {
		dimacsCstrs << *itCl << " ";
	}
	dimacsCstrs << "0\n";
	++nCstrs;
	return true;
}


std::vector<int>& ExternalSatSolver::propagatedAtDecisionLvlZero() {
	std::cerr << "operation unavailable for external solver" << std::endl;
	exit(1);
}


bool ExternalSatSolver::isPropagatedAtDecisionLvlZero(int lit) {
	std::cerr << "operation unavailable for external solver" << std::endl;
	exit(1);
}


void ExternalSatSolver::computeModel() {
	std::vector<int> assumps;
	computeModel(assumps);
}


void ExternalSatSolver::computeModel(std::vector<int> &assumps) {
	char *tmpname = strdup("/tmp/tmp_CoQuiASS_ext_XXXXXX");
	if(-1==mkstemp(tmpname)) {
		perror("ExternalSatBasedSolver::hasAModel::mkstemp");
		exit(2);
	}
	std::ofstream f(tmpname);
	f << "p cnf " << this->nVars << " " << (unsigned int)(this->nCstrs+assumps.size()) << std::endl;
	f << dimacsCstrs.str();
	for(std::vector<int>::iterator it = assumps.begin(); it != assumps.end(); ++it)
		f << *it << " 0" << std::endl;
	f.close();
	launchExternalSolver(std::string(tmpname));
	unlink(tmpname);
	free(tmpname);
}


static char *convertToCString(const std::string & s) {
	char *pc = new char[s.size()+1];
	std::strcpy(pc, s.c_str());
	return pc;
}


void ExternalSatSolver::launchExternalSolver(std::string instanceFile) {
	pid_t pid;
	int pfds[2];

	if(-1 == pipe(pfds)) {perror("CoQuiAAS");exit(1);}
	if(-1 == (pid = fork())) {perror("CoQuiAAS");exit(1);}
	if(pid > 0) {
		handleForkAncestor(pfds);
	} else {
		handleForkChild(instanceFile, pfds);
	}
}


void ExternalSatSolver::handleForkAncestor(int pfds[]) {
	wait(NULL);
	close(pfds[1]);
	FILE *childOutFile = fdopen(pfds[0],"r");
	char buffer[EXTERNAL_SAT_BUFFER_SIZE];
	while(fgets(buffer, EXTERNAL_SAT_BUFFER_SIZE, childOutFile)) {
		if(!strncmp(buffer, "s UNSATISFIABLE", 15)) {
			return;
		}
		if(!strncmp(buffer, "v ", 2)) {
			extractModel(buffer, childOutFile);
		}
	}
	fclose(childOutFile);
	close(pfds[0]);
}


void ExternalSatSolver::extractModel(char buffer[], FILE *childOutFile) {
	std::vector<bool> model;
	model.resize(this->nVars);
	char *pc = buffer+2;
	int nb = 0;
	bool readingNb = false;
	bool minus = false;
	for(; *pc != '\n'; ++pc) {
		if(!*pc) {
			fgets(buffer, EXTERNAL_SAT_BUFFER_SIZE, childOutFile);
			pc = buffer;
		}
		if((*pc == ' ' || *pc == '\t') && readingNb) {
			if(!nb) break;
			model[nb-1] = !minus;
			minus = false;
			nb = 0;
			readingNb = false;
		} else if(*pc >= '0' && *pc <= '9') {
			nb = 10*nb + (*pc - '0');
			readingNb = true;
		} else if(*pc == '-') {
			minus = true;
		}
	}
	this->models.push_back(model);
}


void ExternalSatSolver::handleForkChild(std::string instanceFile, int pfds[]) {
	std::vector<std::string> tokens;
	std::istringstream iss(this->command);
	std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), back_inserter(tokens));
	std::vector<char*> ctokens;
	std::transform(tokens.begin(), tokens.end(), std::back_inserter(ctokens), convertToCString);
	char **args = new char*[ctokens.size()+1];
	for(unsigned int i=0; i<ctokens.size(); ++i) {
		if(!strcmp(ctokens[i],"FILE"))
			args[i]= strdup(instanceFile.c_str());
		else
			args[i] = ctokens[i];
	}
	args[ctokens.size()] = (char*)NULL;
	close(1);
	if(-1==dup(pfds[1])) {
		perror("launchExternalSolver::dup");
		exit(2);
	}
	close(pfds[0]);
	execvp(args[0], args);
	perror("CoQuiAAS");
	exit(1);
}


void ExternalSatSolver::computeAllModels() {
	std::vector<int> assumps;
	return computeAllModels(assumps);
}


void ExternalSatSolver::computeAllModels(std::vector<int> &assumps) {
	unsigned int nbModels = 0;
	for(;;) {
		computeModel(assumps);
		if(this->models.size() > nbModels) {
			addBlockingClause();
			++nbModels;
		} else {
			break;
		}
	}
}


void ExternalSatSolver::addBlockingClause() {
	std::vector<bool> model = this->models[this->models.size() - 1];
	std::vector<int> intCl;
	for(int i=0; i<this->nVars; ++i) {
		intCl.push_back(model[i] ? -(i+1) : i+1);
	}
	addClause(intCl);
}


bool ExternalSatSolver::hasAModel() {
	return this->models.size() > 0;
}


std::vector<bool>& ExternalSatSolver::getModel() {
	return this->models[0];
}


std::vector<std::vector<bool> >& ExternalSatSolver::getModels() {
	return this->models;
}

ExternalSatSolver::~ExternalSatSolver() {}

