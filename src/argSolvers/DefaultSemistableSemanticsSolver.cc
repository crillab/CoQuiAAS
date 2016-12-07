/*
 * DefaultSemistableSemanticsSolver.cc
 *
 *  Created on: 5 déc. 2016
 *      Author: lonca
 */


#include "DefaultSemistableSemanticsSolver.h"


using namespace CoQuiAAS;


DefaultSemistableSemanticsSolver::DefaultSemistableSemanticsSolver(MaxSatSolver &solver, Attacks &attacks, VarMap &varMap, TaskType taskType)  : SemanticsProblemSolver(attacks, varMap, taskType), solver(solver) {}


void DefaultSemistableSemanticsSolver::init() {
	MaxSatEncodingHelper helper(solver, attacks, varMap);
	int disjId = helper.reserveDisjunctionVars();
	helper.setMaxRangeNeeded(disjId);
	helper.createAttackersDisjunctionVars(disjId);
	helper.createCompleteEncodingConstraints(disjId);
}


void DefaultSemistableSemanticsSolver::computeOneExtension() {
	solver.computeMaxSat();
	if(!solver.hasAModel()) {
		this->answer = "NO";
	}
	std::vector<bool> ms = solver.getMaxSat();
	this->answer = modelToString(ms);
}


void DefaultSemistableSemanticsSolver::computeAllExtensions() {
	std::vector<std::vector<bool> > allMs = computeAllMaxInclExtensions();
	this->answer = "[";
	int nMs = (signed) allMs.size();
	for(int i=0; i<nMs-1; ++i) {
		this->answer = this->answer + modelToString(allMs[i]) + ",";
	}
	if(nMs) this->answer = this->answer + modelToString(allMs[allMs.size()-1]);
	this->answer = this->answer + "]";
}


std::vector<std::vector<bool> > DefaultSemistableSemanticsSolver::computeAllMaxInclExtensions() {
	std::vector<std::vector<bool> > maxExts;
	std::vector<int> assumps;
	std::vector<int> selectors;
	for(;;) {
		solver.computeMaxSat(assumps);
		if(!solver.hasAModel()) break;
		std::vector<bool> maxExt = solver.getMaxSat();
		maxExts.push_back(maxExt);
		std::vector<int> blockingCl;
		for(int i=0; i<(int) maxExt.size(); ++i) {
			if(!maxExt[i]) {
				blockingCl.push_back(i+1);
			}
		}
		int selector = solver.addSelectedClause(blockingCl);
		assumps.push_back(selector);
		selectors.push_back(selector);
	}
	for(int i=0; i<(int) selectors.size(); ++i) {
		std::vector<int> cl;
		cl.push_back(-selectors[i]);
		solver.addClause(cl);
	}
	return maxExts;
}


void DefaultSemistableSemanticsSolver::isCredulouslyAccepted() {
	std::vector<std::vector<bool> > allMs = computeAllMaxInclExtensions();
	int arg = varMap.getVar(this->acceptanceQueryArgument);
	for(unsigned int i=0; i<allMs.size(); ++i) {
		if(allMs[i][arg-1]) {
			this->answer = "YES";
			return;
		}
	}
	this->answer = "NO";
}


void DefaultSemistableSemanticsSolver::isSkepticallyAccepted() {
	std::vector<std::vector<bool> > allMs = computeAllMaxInclExtensions();
	int arg = varMap.getVar(this->acceptanceQueryArgument);
	for(unsigned int i=0; i<allMs.size(); ++i) {
		if(!allMs[i][arg-1]) {
			this->answer = "NO";
			return;
		}
	}
	this->answer = "YES";
}


DefaultSemistableSemanticsSolver::~DefaultSemistableSemanticsSolver() {}


