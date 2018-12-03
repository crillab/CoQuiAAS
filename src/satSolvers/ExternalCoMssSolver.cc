/*
 * ExternalCoMssSolver.cc
 *
 *  Created on: 7 nov. 2017
 *      Author: lonca
 */

#include "ExternalCoMssSolver.h"


using namespace CoQuiAAS;


ExternalCoMssSolver::ExternalCoMssSolver(std::string path) {
	this->path = path;
	this->nSoftCstrs = 0;
	this->nVars = 0;
	this->realNumberOfVars = 0;
	this->nCstrs = 0;
}


ExternalCoMssSolver::~ExternalCoMssSolver() {}


void ExternalCoMssSolver::addSoftClause(std::vector<int> &clause) {
	for(std::vector<int>::iterator itCl = clause.begin(); itCl != clause.end(); ++itCl) {
		this->dimacsSoftCstrs << *itCl << " ";
	}
	this->dimacsSoftCstrs << "0\n";
	++nSoftCstrs;
}


void ExternalCoMssSolver::clearMss() {
	this->mss.clear();
	this->models.clear();
}


bool ExternalCoMssSolver::computeMss() {
	std::vector<int> assumps;
	return computeMss(assumps);
}


bool ExternalCoMssSolver::computeMss(std::vector<int> &assumps) {
	std::string instance = writeInstanceForMSS(assumps);
	int ret = launchExternalSolver(instance, false, NULL);
	unlink(instance.c_str());
	return ret;
}


void ExternalCoMssSolver::computeAllMss(std::function<void(std::vector<int>&)> callback) {
	std::vector<int> assumps;
	computeAllMss(callback, assumps);
}


void ExternalCoMssSolver::computeAllMss(std::function<void(std::vector<int>&)> callback, std::vector<int> &assumps) {
  this->shouldStopMssEnum = false;
	std::string instance = writeInstanceForMSS(assumps);
	launchExternalSolver(instance, true,  callback);
	unlink(instance.c_str());
}


void ExternalCoMssSolver::stopMssEnum() {
  this->shouldStopMssEnum = true;
}


bool ExternalCoMssSolver::hasAMss() {
	return this->mss.size() > 0;
}


std::vector<int>& ExternalCoMssSolver::getMss() {
	return this->mss[this->mss.size()-1];
}


std::vector<std::vector<int> >& ExternalCoMssSolver::getAllMss() {
	return this->mss;
}

void ExternalCoMssSolver::resetAllMss() {
	this->mss.clear();
}

void ExternalCoMssSolver::addVariables(int nVars) {
	addVariables(nVars, false);
}

void ExternalCoMssSolver::addVariables(int nVars, bool auxVar) {
	this->nVars += nVars;
	if(auxVar) this->realNumberOfVars += nVars;
}


bool ExternalCoMssSolver::addClause(std::vector<int> &clause) {
	for(std::vector<int>::iterator itCl = clause.begin(); itCl != clause.end(); ++itCl) {
		dimacsCstrs << *itCl << " ";
	}
	dimacsCstrs << "0\n";
	++nCstrs;
	return true;
}


int ExternalCoMssSolver::addSelectedClause(std::vector<int> &clause) {
	addVariables(1, true);
	int selector = this->nVars;
	clause.push_back(-selector);
	addClause(clause);
	return selector;
}


std::vector<int>& ExternalCoMssSolver::propagatedAtDecisionLvlZero() {
	std::cerr << "operation unavailable for external solver" << std::endl;
	exit(1);
}


bool ExternalCoMssSolver::isPropagatedAtDecisionLvlZero(int lit) {
	std::cerr << "operation unavailable for external solver" << std::endl;
	exit(1);
}


bool ExternalCoMssSolver::computeModel() {
	std::vector<int> assumps;
	return computeModel(assumps);
}


bool ExternalCoMssSolver::computeModel(std::vector<int> &assumps) {
	auto oldNSoftCstrs = this->nSoftCstrs;
	this->nSoftCstrs = 2*realNumberOfVars;
	std::string instance = writeInstanceForSAT(assumps, this->realNumberOfVars);
	int ret = launchExternalSolver(instance, false, NULL);
	unlink(instance.c_str());
	this->nSoftCstrs = oldNSoftCstrs;
	return ret;
}


void ExternalCoMssSolver::computeAllModels(std::function<void(std::vector<bool>&)> callback) {
	std::vector<int> assumps;
	return computeAllModels(callback, assumps);
}


void ExternalCoMssSolver::computeAllModels(std::function<void(std::vector<bool>&)> callback, std::vector<int> &assumps) {
	auto oldNSoftCstrs = this->nSoftCstrs;
	this->nSoftCstrs = 2*realNumberOfVars;
	std::string instance = writeInstanceForSAT(assumps, this->realNumberOfVars);
	launchExternalSolver(instance, true, [this, callback](std::vector<int>& mss){
    std::vector<bool> model = extractModelFromMss(mss);
    callback(model);
  });
	unlink(instance.c_str());
	this->nSoftCstrs = oldNSoftCstrs;
}


bool ExternalCoMssSolver::hasAModel() {
	return this->models.size() > 0;
}


std::vector<bool>& ExternalCoMssSolver::getModel() {
	return this->models[this->models.size()-1];
}

void ExternalCoMssSolver::resetModels() {
	this->models.clear();
}

std::vector<std::vector<bool> >& ExternalCoMssSolver::getModels() {
	return this->models;
}


std::string ExternalCoMssSolver::writeInstanceForMSS(std::vector<int> assumps) {
	std::string tmpname("/tmp/tmp_CoQuiASS_ext_XXXXXX");
	if(-1==mkstemp((char *) tmpname.c_str())) {
		perror("ExternalCoMssSolver::hasAModel::mkstemp");
		exit(2);
	}
	std::ofstream f(tmpname.c_str());
	int hardCstrWeight = (this->nSoftCstrs+1);
	unsigned int nClauses = this->nSoftCstrs+this->nCstrs+assumps.size();
	f << "p wcnf " << this->nVars << " " << nClauses << " " << hardCstrWeight <<std::endl;
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


std::string ExternalCoMssSolver::writeInstanceForSAT(std::vector<int> assumps, int realNumberOfVars) {
	std::string tmpname("/tmp/tmp_CoQuiASS_ext_XXXXXX");
	if(-1==mkstemp((char *) tmpname.c_str())) {
		perror("ExternalCoMssSolver::hasAModel::mkstemp");
		exit(2);
	}
	std::ofstream f(tmpname.c_str());
	int hardCstrWeight = (2*realNumberOfVars+1);
	unsigned int nClauses = this->nCstrs+assumps.size()+realNumberOfVars;
	f << "p wcnf " << this->nVars << " " << nClauses << " " << hardCstrWeight <<std::endl;
	std::string line;
	std::istringstream hardCstrsStream(this->dimacsCstrs.str());
	for(int i=1; i<=realNumberOfVars; ++i) f << "1 " << i << " 0" << std::endl;
	for(int i=1; i<=realNumberOfVars; ++i) f << "1 " << (-i) << " 0" << std::endl;
	while (std::getline(hardCstrsStream, line)) {
		f << hardCstrWeight << " " << line << std::endl;
	}
	for(std::vector<int>::iterator it = assumps.begin(); it != assumps.end(); ++it) {
		f << hardCstrWeight << " " << *it << " 0" << std::endl;
	}
	f.close();
	return tmpname;
}


bool ExternalCoMssSolver::launchExternalSolver(std::string instanceFile, bool allModels, std::function<void(std::vector<int>&)> callback) {
  pid_t pid;
  int pfds[2];
  
  if(-1 == pipe(pfds)) {perror("CoQuiAAS");exit(1);}
  if(-1 == (pid = fork())) {perror("CoQuiAAS");exit(1);}
  if(pid > 0) {
    return handleForkAncestor(pid, pfds, allModels, true);
  } else {
    handleForkChild(instanceFile, allModels, pfds);
    return true; // will never be returned since exec() is called in fork child
  }
}


void ExternalCoMssSolver::handleForkChild(std::string instanceFile, bool allModels, int pfds[]) {
  close(1);
  close(2);
  if(-1==dup(pfds[1])) {
    perror("launchExternalSolver::dup");
    exit(2);
  }
  close(pfds[0]);
  if(allModels) {
    execl(this->path.c_str(), this->path.c_str(), "-enumBlocked", instanceFile.c_str(), "/dev/stdout", NULL);
  } else {
    execl(this->path.c_str(), this->path.c_str(), "-extractCoMss", instanceFile.c_str(), "/dev/stdout", NULL);
  }
  perror("CoQuiAAS");
  std::cerr << "this error may occur in case coMssExtractor has not been found; check its path is \"" << this->path << "\"" << std::endl;
  exit(1);
}


bool ExternalCoMssSolver::handleForkAncestor(int childId, int pipe[], bool allModels, bool extract) {
  wait(NULL);
  close(pipe[1]);
  bool ret = false;
  FILE *childOutFile = fdopen(pipe[0], "r");
  char buffer[BUF_READ_SIZE];
  while(fgets(buffer, BUF_READ_SIZE, childOutFile)) {
    if(!strncmp(buffer, "c MCS(", 6)) {
      char *begin;
      for(begin=buffer+6; *begin!=' '; ++begin);
      this->mss.push_back(extractCoMss(begin+1));
      ret = true;
      if(this->shouldStopMssEnum) {
					kill(childId, SIGTERM);
					break;
			}
      continue;
    }
    if(!strncmp(buffer, "v ", 2)) {
      this->mss.push_back(extractCoMss(buffer+2));
      ret = true;
      if(this->shouldStopMssEnum) {
					kill(childId, SIGTERM);
					break;
			}
      continue;
    }
  }
  if(extract) extractModelsFromMsses();
  fclose(childOutFile);
  close(pipe[0]);
  return ret;
}


std::vector<int> ExternalCoMssSolver::extractCoMss(char *line) {
  std::vector<int> tmpMss;
  for(int i=1; i<=nSoftCstrs; ++i) tmpMss.push_back(i);
  std::vector<int> mcs = readIntVector(line);
  for(unsigned int i=0; i<mcs.size(); ++i) {
    tmpMss[mcs[i]-1] = -1;
  }
  std::vector<int> newMss;
  for(unsigned int i=0; i<tmpMss.size(); ++i) {
    if(tmpMss[i] > 0) {
      newMss.push_back(tmpMss[i]);
    }
  }
  return newMss;
}


std::vector<int> ExternalCoMssSolver::readIntVector(char *line) {
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
 

void ExternalCoMssSolver::extractModelsFromMsses() {
  for(unsigned int i=0; i<this->mss.size(); ++i) {
    this->models.push_back(extractModelFromMss(this->mss[i]));
  }
}


std::vector<bool> ExternalCoMssSolver::extractModelFromMss(std::vector<int> mss) {
  std::vector<int> assumps;
  int oldNSoftCstrs = this->nSoftCstrs;
  this->nSoftCstrs = 2*this->realNumberOfVars;
  std::string f = writeInstanceForSAT(assumps, this->realNumberOfVars, mss);
  pid_t pid;
  int pfds[2];
  if(-1 == pipe(pfds)) {perror("CoQuiAAS");exit(1);}
  if(-1 == (pid = fork())) {perror("CoQuiAAS");exit(1);}
  if(!pid) {
    handleForkChild(f, true, pfds);
  }
  handleForkAncestor(pid, pfds, true, false);
  std::vector<int>& lastMss = this->mss.back();
  std::vector<bool> newModel;
  for(int i=0; i<this->realNumberOfVars; ++i) {
    newModel.push_back(lastMss[i]&1);
  }
  this->mss.pop_back();
  this->nSoftCstrs = oldNSoftCstrs;
  return newModel;
}


std::string ExternalCoMssSolver::writeInstanceForSAT(std::vector<int> assumps, int realNumberOfVars, std::vector<int> mssAssumps) {
  std::string tmpname("/tmp/tmp_CoQuiASS_ext_XXXXXX");
  if(-1==mkstemp((char *) tmpname.c_str())) {
    perror("ExternalCoMssSolver::hasAModel::mkstemp");
    exit(2);
  }
  std::ofstream f(tmpname.c_str());
  int hardCstrWeight = (2*realNumberOfVars+1);
  unsigned int nClauses = this->realNumberOfVars*2 + this->nCstrs + mssAssumps.size() + assumps.size();
  f << "p wcnf " << this->nVars << " " << nClauses << " " << hardCstrWeight <<std::endl;
  std::string line;
  for(int i=1; i<=realNumberOfVars; ++i) {
    f << "1 " << i << " 0" << std::endl;
    f << "1 " << (-i) << " 0" << std::endl;
  }
  std::istringstream hardCstrsStream(this->dimacsCstrs.str());
  std::istringstream softCstrsStream(this->dimacsSoftCstrs.str());
  int cpt=0;
  int mssAssumpsIndex=0;
  while (std::getline(softCstrsStream, line)) {
    if(mssAssumpsIndex >= (signed)mssAssumps.size()) break;
    ++cpt;
    if(mssAssumps[mssAssumpsIndex] == cpt) {
      f << hardCstrWeight << " " << line << std::endl;
      ++mssAssumpsIndex;
    }
  }
  while (std::getline(hardCstrsStream, line)) {
    f << hardCstrWeight << " " << line << std::endl;
  }
  for(std::vector<int>::iterator it = assumps.begin(); it != assumps.end(); ++it) {
    f << hardCstrWeight << " " << *it << " 0" << std::endl;
  }
  f.close();

  // std::cout << "p wcnf " << this->nVars << " " << nClauses << " " << hardCstrWeight <<std::endl;
  // std::string line2;
  // for(int i=1; i<=realNumberOfVars; ++i) {
  //   std::cout << "1 " << i << " 0" << std::endl;
  //   std::cout << "1 " << (-i) << " 0" << std::endl;
  // }
  // std::istringstream hardCstrsStream2(this->dimacsCstrs.str());
  // std::istringstream softCstrsStream2(this->dimacsSoftCstrs.str());
  // int cpt2=0;
  // int mssAssumpsIndex2=0;
  // while (std::getline(softCstrsStream2, line2)) {
  //   if(mssAssumpsIndex2 >= (signed)mssAssumps.size()) break;
  //   ++cpt2;
  //   if(mssAssumps[mssAssumpsIndex2] == cpt2) {
  //     std::cout << hardCstrWeight << " " << line2 << std::endl;
  //     ++mssAssumpsIndex2;
  //   }
  // }
  // while (std::getline(hardCstrsStream2, line2)) {
  //   std::cout << hardCstrWeight << " " << line2 << std::endl;
  // }
  // for(std::vector<int>::iterator it = assumps.begin(); it != assumps.end(); ++it) {
  //   std::cout << hardCstrWeight << " " << *it << " 0" << std::endl;
  // }
  
  return tmpname;
}
