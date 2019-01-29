/*
 * DefaultIdealSemanticsSolver.cc
 *
 *  Created on: 6 déc. 2016
 *      Author: lonca
 */


#include "DefaultIdealSemanticsSolver.h"


using namespace CoQuiAAS;


DefaultIdealSemanticsSolver::DefaultIdealSemanticsSolver(std::shared_ptr<MssSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter):
	SemanticsProblemSolver(attacks, varMap, taskType, formatter), solver(solver) {}


void DefaultIdealSemanticsSolver::init() {
	this->helper = new MssEncodingHelper(solver, attacks, varMap);
	this->helper->setMaxExtensionNeeded();
	int disjId = this->helper->reserveDisjunctionVars();
	this->helper->createAttackersDisjunctionVars(disjId);
	this->helper->createCompleteEncodingConstraints(disjId);
}


void DefaultIdealSemanticsSolver::computeOneExtension() {
	clock_t startTime = clock();
	std::vector<int> mss = justComputeOneExtension();
	this->formatter.writeSingleExtension(mss);
	logSingleExtTime(startTime);
}


std::vector<int> DefaultIdealSemanticsSolver::justComputeOneExtension() {
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	std::vector<std::vector<int> > allMss;
	std::vector<bool> argAllowed(varMap.nVars(), true);
	bool noAllowedArgs = false;
	solver->computeAllMss([this, &allMss, &argAllowed, &noAllowedArgs](std::vector<int>& mss, std::vector<bool>& model){
		allMss.push_back(mss);
		std::vector<bool> argInMss(varMap.nVars(), false);
		bool noneAllowed = true;
		for(unsigned int j=0; j<mss.size(); ++j) {
			argInMss[mss[j]-1] = true;
		}
		for(int j=0; j<varMap.nVars(); ++j) {
			bool allowed = argAllowed[j]&argInMss[j];
			argAllowed[j] = allowed;
			noneAllowed &= !allowed;
		}
		if(noneAllowed) {
			this->solver->stopMssEnum();
			noAllowedArgs = true;
		}
	}, dynAssumps);
	if(allMss.size() == 1) return allMss[0];
	if(noAllowedArgs) {
		std::vector<int> empty;
		return empty;
	}
	std::vector<int> assumps;
	for(int i=0; i<varMap.nVars(); ++i) {
		if(!argAllowed[i]) {
			assumps.push_back(-i-1);
		}
	}
	for(unsigned int i=0; i<dynAssumps.size(); ++i) assumps.push_back(dynAssumps[i]);
	solver->computeMss(assumps);
	return solver->getMss();
}


void DefaultIdealSemanticsSolver::computeAllExtensions() {
	clock_t startTime = clock();
	std::vector<int> mss = justComputeOneExtension();
	this->formatter.writeExtensionListBegin();
	this->formatter.writeExtensionListElmt(mss, true);
	this->formatter.writeExtensionListEnd();
	logOneExtTime(startTime, 1);
	logNoMoreExts(startTime);
}


void DefaultIdealSemanticsSolver::isCredulouslyAccepted() {
	clock_t startTime = clock();
	std::vector<int> mss = justComputeOneExtension();
	int arg = varMap.getVar(this->acceptanceQueryArgument);
	for(unsigned int j=0; j<mss.size(); ++j) {
		if(mss[j] == arg) {
			this->formatter.writeArgAcceptance(true);
			return;
		}
	}
	this->formatter.writeArgAcceptance(false);
	logAcceptanceCheckingTime(startTime);
}


void DefaultIdealSemanticsSolver::isSkepticallyAccepted() {
	isCredulouslyAccepted();
}


DefaultIdealSemanticsSolver::~DefaultIdealSemanticsSolver() {}


