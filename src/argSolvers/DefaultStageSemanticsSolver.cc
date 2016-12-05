/*
 * DefaultStageSemanticsSolver.cc
 *
 *  Created on: 5 déc. 2016
 *      Author: lonca
 */



#include "DefaultStageSemanticsSolver.h"


using namespace CoQuiAAS;


DefaultStageSemanticsSolver::DefaultStageSemanticsSolver(MaxSatSolver &solver, Attacks &attacks, VarMap &varMap, TaskType taskType)  : SemanticsProblemSolver(attacks, varMap, taskType), solver(solver) {}


void DefaultStageSemanticsSolver::init() {
	MaxSatEncodingHelper helper(solver, attacks, varMap);
	int disjId = helper.reserveDisjunctionVars();
	helper.setMaxRangeNeeded(disjId);
	helper.createAttackersDisjunctionVars(disjId);
	helper.createConflictFreenessEncodingConstraints(disjId);
}


void DefaultStageSemanticsSolver::computeOneExtension() {
	solver.computeMaxSat();
	if(!solver.hasAModel()) {
		this->answer = "NO";
	}
	std::vector<bool> ms = solver.getMaxSat();
	this->answer = modelToString(ms);
}


void DefaultStageSemanticsSolver::computeAllExtensions() {
	solver.computeAllMaxSat();
	if(!solver.hasAModel()) {
		this->answer = "[]";
	}
	this->answer = "[";
	std::vector<std::vector<bool> > allMs = solver.getAllMaxSat();
	int nMs = (signed) allMs.size();
	for(int i=0; i<nMs-1; ++i) {
		this->answer = this->answer + modelToString(allMs[i]) + ",";
	}
	if(nMs) this->answer = this->answer + modelToString(allMs[allMs.size()-1]);
	this->answer = this->answer + "]";
}


void DefaultStageSemanticsSolver::isCredulouslyAccepted() {
	std::vector<int> assumps;
	assumps.push_back(varMap.getVar(this->acceptanceQueryArgument));
	solver.computeModel(assumps);
	int optWith = solver.getOptValue();
	assumps.pop_back();
	assumps.push_back(-varMap.getVar(this->acceptanceQueryArgument));
	solver.computeModel(assumps);
	int optWithout = solver.getOptValue();
	this->answer = optWith == optWithout ? "YES" : "NO";
}


void DefaultStageSemanticsSolver::isSkepticallyAccepted() {
	solver.computeAllMaxSat();
		std::vector<std::vector<bool> > allMs = solver.getAllMaxSat();
		int arg = varMap.getVar(this->acceptanceQueryArgument);
		for(unsigned int i=0; i<allMs.size(); ++i) {
			if(!allMs[i][arg-1]) {
				this->answer = "NO";
				return;
			}
		}
		this->answer = "YES";
}


DefaultStageSemanticsSolver::~DefaultStageSemanticsSolver() {}


