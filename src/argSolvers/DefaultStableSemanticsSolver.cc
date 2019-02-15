/*
 * DefaultStableSemanticsSolver.cc
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */


#include "DefaultStableSemanticsSolver.h"


using namespace CoQuiAAS;


DefaultStableSemanticsSolver::DefaultStableSemanticsSolver(std::shared_ptr<SatSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter):
	SemanticsProblemSolver(attacks, varMap, taskType, formatter), solver(solver) {
	this->solver->setBlockingClauseFunction([this](std::vector<bool>& model) -> std::vector<int> {
		std::vector<int> intCl;
		for(int i=0; i<this->problemReducer->getReducedMap()->nVars(); ++i) {
			if(model[i]) intCl.push_back(-i-1);
		}
		return intCl;
	});
}


void DefaultStableSemanticsSolver::init() {
	this->problemReducer = std::make_unique<StableEncodingSatProblemReducer>(varMap, attacks);
	this->problemReducer->search();
	VarMap &reducedMap = *this->problemReducer->getReducedMap().get();
	this->formatter.setVarMap(reducedMap);
	this->helper = new SatEncodingHelper(solver, attacks, reducedMap);
	this->helper->createStableEncodingConstraints();
}


void DefaultStableSemanticsSolver::computeOneExtension() {
	clock_t startTime = clock();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	solver->computeModel(dynAssumps);
	if(!solver->hasAModel()) {
		this->formatter.writeNoExt();
		return;
	}
	std::vector<bool> model = solver->getModel();
	this->formatter.writeSingleExtension(model);
	logSingleExtTime(startTime);
}


void DefaultStableSemanticsSolver::computeAllExtensions() {
	clock_t globalStartTime = clock();
	this->formatter.writeExtensionListBegin();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	int extIndex = 1;
	clock_t startTime = clock();
	solver->computeAllModels([this, &extIndex, &startTime](std::vector<bool>& model){
		this->formatter.writeExtensionListElmt(model, extIndex == 1);
		logOneExtTime(startTime, extIndex);
		extIndex++;
	}, dynAssumps);
	logNoMoreExts(startTime);
	this->formatter.writeExtensionListEnd();
	logAllExtsTime(globalStartTime);
}


void DefaultStableSemanticsSolver::isCredulouslyAccepted() {
	clock_t startTime = clock();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	int argAssump = this->problemReducer->getReducedMap()->getVar(this->problemReducer->translateAcceptanceQueryArgument(this->acceptanceQueryArgument));
	dynAssumps.push_back(argAssump);
	solver->computeModel(dynAssumps);
	this->formatter.writeArgAcceptance(solver->hasAModel());
	logAcceptanceCheckingTime(startTime);
}


void DefaultStableSemanticsSolver::isSkepticallyAccepted() {
	clock_t startTime = clock();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	int argAssump = -this->problemReducer->getReducedMap()->getVar(this->problemReducer->translateAcceptanceQueryArgument(this->acceptanceQueryArgument));
	dynAssumps.push_back(argAssump);
	solver->computeModel(dynAssumps);
	this->formatter.writeArgAcceptance(!solver->hasAModel());
	logAcceptanceCheckingTime(startTime);
}


DefaultStableSemanticsSolver::~DefaultStableSemanticsSolver() {}

