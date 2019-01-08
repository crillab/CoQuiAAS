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
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	solver->computeMss(dynAssumps);
	if(!solver->hasAMss()) {
		this->formatter.writeNoExt();
		return;
	}
	std::vector<int> mss = solver->getMss();
	this->formatter.writeSingleExtension(mss);
}


void DefaultPreferredSemanticsSolver::computeAllExtensions() {
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	this->formatter.writeExtensionListBegin();
	bool first = true;
	solver->computeAllMss([this, &first](std::vector<int>& mss, std::vector<bool>& model){
		this->formatter.writeExtensionListElmt(mss, first);
		first = false;
	}, dynAssumps);
	this->formatter.writeExtensionListEnd();
}


void DefaultPreferredSemanticsSolver::isCredulouslyAccepted() {
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	dynAssumps.push_back(varMap.getVar(this->acceptanceQueryArgument));
	solver->computeModel(dynAssumps);
	this->formatter.writeArgAcceptance(solver->hasAModel());
}


void DefaultPreferredSemanticsSolver::isSkepticallyAccepted() {
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	solver->computeAllMss(NULL, dynAssumps); // TODO stop during search if needed
	std::vector<std::vector<int> > allMss = solver->getAllMss();
	int arg = varMap.getVar(this->acceptanceQueryArgument);
	for(unsigned int i=0; i<allMss.size(); ++i) {
		std::vector<int> mss = allMss[i];
		/*std::cerr << "mss:";
		for(int i=0; i<mss.size(); ++i) {
			std::cerr << " " << mss[i];
		}
		std::cerr << std::endl;*/
		bool found = false;
		for(unsigned int j=0; j<mss.size(); ++j) {
			if(mss[j] == arg) {
				found = true;
				break;
			}
		}
		if(!found) {
			this->formatter.writeArgAcceptance(false);
			return;
		}
	}
	this->formatter.writeArgAcceptance(true);
}


DefaultPreferredSemanticsSolver::~DefaultPreferredSemanticsSolver() {}

