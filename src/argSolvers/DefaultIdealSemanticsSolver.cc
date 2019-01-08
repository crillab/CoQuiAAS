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
	std::vector<int> mss = justComputeOneExtension();
	this->formatter.writeSingleExtension(mss);
}


std::vector<int> DefaultIdealSemanticsSolver::justComputeOneExtension() {
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	solver->computeAllMss(NULL, dynAssumps);
	std::vector<std::vector<int> > allMss = solver->getAllMss();
	// TODO if one MSS, return it
	std::vector<bool> argAllowed(varMap.nVars(), true);
	int nMss = (signed) allMss.size();
	for(int i=0; i<nMss; ++i) {
		std::vector<int> mss = allMss[i];
		std::vector<bool> argInMss(varMap.nVars(), false);
		int mssSize = (int) mss.size();
		for(int j=0; j<mssSize; ++j) {
			argInMss[mss[j]-1] = true;
		}
		for(int j=0; j<varMap.nVars(); ++j) {
			argAllowed[j] = argAllowed[j]&argInMss[j];
		}
	}
	// TODO if no allowed args, return empty extension
	std::vector<int> assumps;
	for(int i=0; i<varMap.nVars(); ++i) {
		if(!argAllowed[i]) {
			assumps.push_back(-i-1);
		}
	}
	solver->computeMss(assumps);
	return solver->getMss();
}


void DefaultIdealSemanticsSolver::computeAllExtensions() {
	std::vector<int> mss = justComputeOneExtension();
	this->formatter.writeExtensionListBegin();
	this->formatter.writeExtensionListElmt(mss, true);
	this->formatter.writeExtensionListEnd();
}


void DefaultIdealSemanticsSolver::isCredulouslyAccepted() {
	std::vector<int> mss = justComputeOneExtension();
	int arg = varMap.getVar(this->acceptanceQueryArgument);
	for(unsigned int j=0; j<mss.size(); ++j) {
		if(mss[j] == arg) {
			this->formatter.writeArgAcceptance(true);
			return;
		}
	}
	this->formatter.writeArgAcceptance(false);
}


void DefaultIdealSemanticsSolver::isSkepticallyAccepted() {
	isCredulouslyAccepted();
}


DefaultIdealSemanticsSolver::~DefaultIdealSemanticsSolver() {}


