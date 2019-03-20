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
	this->problemReducer = std::make_unique<CompleteEncodingSatProblemReducer>(varMap, attacks);
	this->problemReducer->search();
	VarMap &reducedMap = *this->problemReducer->getReducedMap().get();
	this->formatter.setVarMap(reducedMap);
	this->helper = new MssEncodingHelper(solver, attacks, reducedMap);
	this->helper->setMaxExtensionNeeded();
	int disjId = this->helper->reserveDisjunctionVars();
	this->helper->createAttackersDisjunctionVars(disjId);
	this->helper->createCompleteEncodingConstraints(disjId);
}


void DefaultIdealSemanticsSolver::computeOneExtension() {
	clock_t startTime = clock();
	std::vector<int> mss = justComputeOneExtension(NULL);
	this->formatter.writeSingleExtension(mss);
	logSingleExtTime(startTime);
}


std::vector<int> DefaultIdealSemanticsSolver::justComputeOneExtension(std::function<void(std::vector<int>&)> prExtCallback) {
	this->stopSearch = false;
	int nVars = this->problemReducer->getReducedMap()->nVars();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	std::vector<std::vector<int> > allMss;
	std::vector<bool> argAllowed(nVars, true);
	std::vector<int>& propagated = solver->propagatedAtDecisionLvlZero();
	std::vector<bool> grExt = SatSolver::toBoolModel(propagated, this->problemReducer->getReducedMap()->nVars());
	bool isGrounded = false;
	solver->computeAllMss([this, &allMss, &argAllowed, prExtCallback, grExt, &isGrounded](std::vector<int>& mss, std::vector<bool>& model){
		allMss.push_back(mss);
		if(prExtCallback) prExtCallback(mss);
		if(this->stopSearch) this->solver->stopMssEnum();
		std::vector<bool> argInMss(argAllowed.size(), false);
		for(unsigned int j=0; j<mss.size(); ++j) {
			argInMss[mss[j]-1] = true;
		}
		bool onlyGrAllowed = true;
		for(unsigned int j=0; j<argAllowed.size(); ++j) {
			bool allowed = argAllowed[j]&argInMss[j];
			argAllowed[j] = allowed;
			onlyGrAllowed &= (!allowed | grExt[j]);
		}
		if(onlyGrAllowed) {
			isGrounded = true;
			this->solver->stopMssEnum();
		}
	}, dynAssumps);
	if(isGrounded) {
		std::vector<int> gr;
		for(unsigned int i=0; i<grExt.size(); ++i) {
			if(grExt[i]) gr.push_back(1+i);
		}
		return gr;
	}
	if(this->stopSearch) return allMss[0];
	if(allMss.size() == 1) return allMss[0];
	std::vector<int> assumps;
	for(int i=0; i<nVars; ++i) {
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
	std::vector<int> mss = justComputeOneExtension(NULL);
	this->formatter.writeExtensionListBegin();
	this->formatter.writeExtensionListElmt(mss, true);
	this->formatter.writeExtensionListEnd();
	logOneExtTime(startTime, 1);
	logNoMoreExts(startTime);
}


void DefaultIdealSemanticsSolver::isCredulouslyAccepted() {
	clock_t startTime = clock();
	int arg = this->problemReducer->getReducedMap()->getVar(this->problemReducer->translateAcceptanceQueryArgument(this->acceptanceQueryArgument));
	bool status = false;
	std::vector<int>& propagated = solver->propagatedAtDecisionLvlZero();
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
		bool shouldSearch = true;
		std::vector<int> ext = justComputeOneExtension([this,arg,&shouldSearch,&status](std::vector<int>& mss){
			bool localStatus = false;
			for(unsigned int j=0; j<mss.size(); ++j) {
				if(mss[j] == arg) {
					localStatus = true;
					break;
				}
			}
			if(!localStatus) {
				shouldSearch = false;
				status = false;
				this->stopSearch = true;
			}
		});
		if(shouldSearch) {
			for(unsigned int j=0; j<ext.size(); ++j) {
				if(ext[j] == arg) {
					status = true;
					break;
				}
			}
		}
	}
	this->formatter.writeArgAcceptance(status);
	logAcceptanceCheckingTime(startTime);
}


void DefaultIdealSemanticsSolver::isSkepticallyAccepted() {
	isCredulouslyAccepted();
}


DefaultIdealSemanticsSolver::~DefaultIdealSemanticsSolver() {}


