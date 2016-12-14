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
	this->blockingSelectors.clear();
}


bool LbxCoMssSolver::computeMss() {
	std::vector<int> assumps;
	computeMss(assumps);
}


bool LbxCoMssSolver::computeMss(std::vector<int> &assumps);


void LbxCoMssSolver::computeAllMss() {
	std::vector<int> assumps;
	computeAllMss(assumps);
}


void LbxCoMssSolver::computeAllMss(std::vector<int> &assumps);


bool LbxCoMssSolver::hasAMss() {
	return this->mss.size() > 0;
}


std::vector<int>& LbxCoMssSolver::getMss() {
	return this->mss[this->mss.size()-1];
}


std::vector<std::vector<int> >& LbxCoMssSolver::getAllMss() {
	return this->mss;
}

std::string LbxCoMssSolver::writeInstance(std::vector<int> assumps) {
	std::string tmpname("/tmp/tmp_CoQuiASS_ext_XXXXXX");
	if(-1==mkstemp(tmpname.c_str())) {
		perror("ExternalSatBasedSolver::hasAModel::mkstemp");
		exit(2);
	}
	std::ofstream f(tmpname.c_str());
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
		handleForkChild(instanceFile, pfds);
		return true; // will never be returned since exec() is called in fork child
	}
}


void ExternalSatSolver::handleForkChild(std::string instanceFile, bool allModels, int pfds[]) {
	std::vector<std::string> tokens;
	std::istringstream iss(this->command);
	std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), back_inserter(tokens));
	std::vector<char*> ctokens;
	std::transform(tokens.begin(), tokens.end(), std::back_inserter(ctokens), convertToCString);
	char **args = new char*[allModels ? 4 : 6];
	args[0] = this->lbxPath;
	args[1] = "-wm";
	if(!allModels) {
		args[2] = "-n";
		args[3] = "1";
	}
	args[allModels ? 2 : 4] = instanceFile;
	args[allModels ? 3 : 5] = NULL;
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


bool ExternalMssSolver::handleForkAncestor(int pipe[]) {
	int mssFound = false;
	if(computingModel) {
		return ExternalSatSolver::handleForkAncestor(pipe);
	}
	wait(NULL);
	close(pipe[1]);
	FILE *childOutFile = fdopen(pipe[0], "r");
	char buffer[EXTERNAL_SAT_BUFFER_SIZE];
	while(fgets(buffer, EXTERNAL_SAT_BUFFER_SIZE, childOutFile)) {
		if(this->outInspector.isHardPartStatusLine(buffer)) {
			if(!this->outInspector.getHardPartStatus(buffer)) break;
		}
		if(this->outInspector.isCoMssLine(buffer)) {
			this->mss.push_back(this->outInspector.getCoMss(buffer, nSoftCstrs, childOutFile));
			mssFound = true;
		}
	}
	fclose(childOutFile);
	close(pipe[0]);
	return mssFound;
}
