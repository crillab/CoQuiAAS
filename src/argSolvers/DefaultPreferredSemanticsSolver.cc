/*
 * DefaultPreferredSemanticsSolver.cpp
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */


#include "DefaultPreferredSemanticsSolver.h"
#include "BuiltInMssSolverNG.h"


using namespace CoQuiAAS;


DefaultPreferredSemanticsSolver::DefaultPreferredSemanticsSolver(std::shared_ptr<MssSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter):
	SemanticsProblemSolver(attacks, varMap, taskType, formatter), solver(solver) {}


void DefaultPreferredSemanticsSolver::init() {
	this->problemReducer = std::make_unique<CompleteEncodingSatProblemReducer>(varMap, attacks);
	this->problemReducer->search();
	VarMap &reducedMap = *this->problemReducer->getReducedMap().get();
	this->formatter.setVarMap(reducedMap);
	this->solver->setStoreLearnts(false);
	this->helper = new MssEncodingHelper(solver, attacks, reducedMap);
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
	solver->resetAllMss();
	solver->resetModels();
	logSingleExtTime(startTime);
}


void DefaultPreferredSemanticsSolver::computeAllExtensions() {
	clock_t globalStartTime = clock();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	this->formatter.writeExtensionListBegin();
	int extIndex = 1;
	clock_t startTime = clock();
	std::vector<int>& propagated = solver->propagatedAtDecisionLvlZero(dynAssumps);
	std::vector<bool> grExt = SatSolver::toBoolModel(propagated, this->problemReducer->getReducedMap()->nVars());
	solver->computeAllMss([this, &extIndex, &startTime, grExt](std::vector<int>& mss, std::vector<bool>& model){
		this->formatter.writeExtensionListElmt(mss, extIndex == 1);
		if(extIndex == 1) {
			bool isGrounded = true;
			for(unsigned int i=0; i<this->problemReducer->getReducedMap()->nVars(); ++i) {
				if(grExt[i] != model[i]) {
					isGrounded = false;
					break;
				}
			}
			if(isGrounded) {
				solver->stopMssEnum();
			}
		}
		extIndex++;
		startTime = clock();
	}, dynAssumps);
	solver->resetAllMss();
	solver->resetModels();
	logNoMoreExts(startTime);
	this->formatter.writeExtensionListEnd();
	logAllExtsTime(globalStartTime);
}


void DefaultPreferredSemanticsSolver::isCredulouslyAccepted() {
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
	solver->resetModels();
	logAcceptanceCheckingTime(startTime);
}


void DefaultPreferredSemanticsSolver::isSkepticallyAccepted() {
	clock_t startTime = clock();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	bool status = true;
	int arg = this->problemReducer->getReducedMap()->getVar(this->problemReducer->translateAcceptanceQueryArgument(this->acceptanceQueryArgument));
	std::vector<int>& propagated = solver->propagatedAtDecisionLvlZero(dynAssumps);
	bool isPropagated = false;
	bool propagatedValue = false;
	for(unsigned int i=0; i<propagated.size(); ++i) {
		if(propagated[i] == arg) {
			isPropagated = true;
			propagatedValue = true;
			break;
		} else if(propagated[i] == -arg) {
			isPropagated = true;
			propagatedValue = false;
			break;
		}
	}
	if(isPropagated) {
		status = propagatedValue;
	} else {
		this->solver->computeAllMss([this,arg,&status](std::vector<int>& mss, std::vector<bool>& model){
			if(!model[arg-1]) {
				status = false;
				this->solver->stopMssEnum();
			}
		}, dynAssumps);
		solver->resetAllMss();
		solver->resetModels();
	}
	this->formatter.writeArgAcceptance(status);
	logAcceptanceCheckingTime(startTime);
}


DefaultPreferredSemanticsSolver::~DefaultPreferredSemanticsSolver() {}

