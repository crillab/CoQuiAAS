/*
 * DefaultPreferredSemanticsSolver.cpp
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */


#include "DefaultPreferredSemanticsSolver.h"


using namespace CoQuiAAS;


DefaultPreferredSemanticsSolver::DefaultPreferredSemanticsSolver(std::shared_ptr<MssSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter):
	SemanticsProblemSolver(attacks, varMap, taskType, formatter), solver(solver) {}


void DefaultPreferredSemanticsSolver::init() {
	this->helper = new MssEncodingHelper(solver, attacks, varMap);
	switch(taskType) {
	case TASK_CRED_INF:
		break;
	default:
		this->helper->setMaxExtensionNeeded();
	}
	int disjId = this->helper->reserveDisjunctionVars();
	this->helper->createAttackersDisjunctionVars(disjId);
	this->helper->createCompleteEncodingConstraints(disjId);
}


void DefaultPreferredSemanticsSolver::computeOneExtension() {
	clock_t startTime = clock();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	solver->computeMss(dynAssumps);
	if(!solver->hasAMss()) {
		this->formatter.writeNoExt();
		return;
	}
	std::vector<int> mss = solver->getMss();
	this->formatter.writeSingleExtension(mss);
	logSingleExtTime(startTime);
}


void DefaultPreferredSemanticsSolver::computeAllExtensions() {
	clock_t globalStartTime = clock();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	this->formatter.writeExtensionListBegin();
	int extIndex = 1;
	clock_t startTime = clock();
	solver->computeAllMss([this, &extIndex, &startTime](std::vector<int>& mss, std::vector<bool>& model){
		this->formatter.writeExtensionListElmt(mss, extIndex == 1);
		extIndex++;
		startTime = clock();
	}, dynAssumps);
	logNoMoreExts(startTime);
	this->formatter.writeExtensionListEnd();
	logAllExtsTime(globalStartTime);
}


void DefaultPreferredSemanticsSolver::isCredulouslyAccepted() {
	clock_t startTime = clock();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	dynAssumps.push_back(varMap.getVar(this->acceptanceQueryArgument));
	solver->computeModel(dynAssumps);
	this->formatter.writeArgAcceptance(solver->hasAModel());
	logAcceptanceCheckingTime(startTime);
}


void DefaultPreferredSemanticsSolver::isSkepticallyAccepted() {
	clock_t startTime = clock();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	bool status = true;
	int arg = varMap.getVar(this->acceptanceQueryArgument);
	this->solver->computeAllMss([this,arg,&status](std::vector<int>& mss, std::vector<bool>& model){
		if(!model[arg-1]) {
			status = false;
			this->solver->stopMssEnum();
		}
	}, dynAssumps);
	this->formatter.writeArgAcceptance(status);
	logAcceptanceCheckingTime(startTime);
}


DefaultPreferredSemanticsSolver::~DefaultPreferredSemanticsSolver() {}

