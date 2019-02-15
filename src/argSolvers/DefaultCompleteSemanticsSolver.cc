/*
 * DefaultCompleteSemanticsSolver.cc
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */


#include "DefaultCompleteSemanticsSolver.h"


using namespace CoQuiAAS;


DefaultCompleteSemanticsSolver::DefaultCompleteSemanticsSolver(std::shared_ptr<SatSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter):
	SemanticsProblemSolver(attacks, varMap, taskType, formatter), solver(solver) {}


void DefaultCompleteSemanticsSolver::init() {
	this->problemReducer = std::make_unique<CompleteEncodingSatProblemReducer>(varMap, attacks);
	VarMap &reducedMap = *this->problemReducer->getReducedMap().get();
	this->formatter.setVarMap(reducedMap);
	this->helper = new SatEncodingHelper(solver, attacks, reducedMap);
	int disjId = this->helper->reserveDisjunctionVars();
	this->helper->createAttackersDisjunctionVars(disjId);
	this->helper->createCompleteEncodingConstraints(disjId);
}


void DefaultCompleteSemanticsSolver::computeOneExtension() {
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


void DefaultCompleteSemanticsSolver::computeAllExtensions() {
	clock_t globalStartTime = clock();
	this->formatter.writeExtensionListBegin();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	int extIndex = 1;
	clock_t startTime = clock();
	solver->computeAllModels([this, &extIndex, &startTime](std::vector<bool>& model){
		this->formatter.writeExtensionListElmt(model, extIndex == 1);
		logOneExtTime(startTime, extIndex);
		extIndex++;
		startTime = clock();
	}, dynAssumps);
	logNoMoreExts(startTime);
	this->formatter.writeExtensionListEnd();
	logAllExtsTime(globalStartTime);
}


void DefaultCompleteSemanticsSolver::isCredulouslyAccepted() {
	clock_t startTime = clock();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	int argAssump = this->problemReducer->getReducedMap()->getVar(this->problemReducer->translateAcceptanceQueryArgument(this->acceptanceQueryArgument));
	dynAssumps.push_back(argAssump);
	solver->computeModel(dynAssumps);
	this->formatter.writeArgAcceptance(solver->hasAModel());
	logAcceptanceCheckingTime(startTime);
}


void DefaultCompleteSemanticsSolver::isSkepticallyAccepted() {
	clock_t startTime = clock();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	int argAssump = -this->problemReducer->getReducedMap()->getVar(this->problemReducer->translateAcceptanceQueryArgument(this->acceptanceQueryArgument));
	dynAssumps.push_back(argAssump);
	solver->computeModel(dynAssumps);
	this->formatter.writeArgAcceptance(!solver->hasAModel());
	logAcceptanceCheckingTime(startTime);
}


DefaultCompleteSemanticsSolver::~DefaultCompleteSemanticsSolver() {
	delete this->helper;
}

