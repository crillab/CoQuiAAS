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
	this->problemReducer->search();
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
	std::vector<int>& propagated = solver->propagatedAtDecisionLvlZero(dynAssumps);
	this->formatter.writeSingleExtension(propagated);
	logSingleExtTime(startTime);
}


void DefaultCompleteSemanticsSolver::computeAllExtensions() {
	clock_t globalStartTime = clock();
	this->formatter.writeExtensionListBegin();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	std::vector<int>& propagated = solver->propagatedAtDecisionLvlZero(dynAssumps);
	std::vector<bool> grExt = SatSolver::toBoolModel(propagated, this->problemReducer->getReducedMap()->nVars());
	int extIndex = 1;
	clock_t startTime = clock();
	solver->computeAllModels([this, &extIndex, &startTime](std::vector<bool>& model){
		this->formatter.writeExtensionListElmt(model, extIndex == 1);
		logOneExtTime(startTime, extIndex);
		extIndex++;
		startTime = clock();
	}, dynAssumps, grExt);
	logNoMoreExts(startTime);
	this->formatter.writeExtensionListEnd();
	logAllExtsTime(globalStartTime);
}


void DefaultCompleteSemanticsSolver::isCredulouslyAccepted() {
	clock_t startTime = clock();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	int argAssump = this->problemReducer->getReducedMap()->getVar(this->problemReducer->translateAcceptanceQueryArgument(this->acceptanceQueryArgument));
	std::vector<int>& propagated = solver->propagatedAtDecisionLvlZero(dynAssumps);
	bool isPropagated = false;
	bool propagatedValue = false;
	for(unsigned int i=0; i<propagated.size(); ++i) {
		if(propagated[i] == argAssump) {
			isPropagated = true;
			propagatedValue = true;
			break;
		} else if(propagated[i] == -argAssump) {
			isPropagated = true;
			propagatedValue = false;
			break;
		}
	}
	if(isPropagated) {
		this->formatter.writeArgAcceptance(propagatedValue);
	} else {
		dynAssumps.push_back(argAssump);
		solver->computeModel(dynAssumps);
		this->formatter.writeArgAcceptance(solver->hasAModel());
	}
	logAcceptanceCheckingTime(startTime);
}


void DefaultCompleteSemanticsSolver::isSkepticallyAccepted() {
	clock_t startTime = clock();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	int argAssump = this->problemReducer->getReducedMap()->getVar(this->problemReducer->translateAcceptanceQueryArgument(this->acceptanceQueryArgument));
	std::vector<int>& propagated = solver->propagatedAtDecisionLvlZero(dynAssumps);
	bool isPropagatedToTrue = false;
	for(unsigned int i=0; i<propagated.size(); ++i) {
		if(propagated[i] == argAssump) {
			isPropagatedToTrue = true;
			break;
		} else if(propagated[i] == -argAssump) {
			break;
		}
	}
	this->formatter.writeArgAcceptance(isPropagatedToTrue);
	logAcceptanceCheckingTime(startTime);
}


DefaultCompleteSemanticsSolver::~DefaultCompleteSemanticsSolver() {
	delete this->helper;
}

