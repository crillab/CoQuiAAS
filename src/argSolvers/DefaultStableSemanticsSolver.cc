/*
 * DefaultStableSemanticsSolver.cc
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */


#include "DefaultStableSemanticsSolver.h"


DefaultStableSemanticsSolver::DefaultStableSemanticsSolver(SatSolver &solver, Attacks &attacks, VarMap &varMap, TaskType taskType)  : SemanticsProblemSolver(attacks, varMap, taskType), solver(solver) {}


void DefaultStableSemanticsSolver::init() {
	SatEncodingHelper helper(solver, attacks, varMap);
	helper.createStableEncodingConstraints();
}


void DefaultStableSemanticsSolver::computeOneExtension() {
	solver.computeModel();
	if(!solver.hasAModel()) {
		this->answer = "NO";
		return;
	}
	std::vector<bool> model = solver.getModel();
	this->answer = modelToString(model);
}


void DefaultStableSemanticsSolver::computeAllExtensions() {
	solver.computeAllModels();
	if(!solver.hasAModel()) {
		this->answer = "[]";
		return;
	}
	this->answer = "[";
	std::vector<vector<bool> > models = solver.getModels();
	int nModels = (signed) models.size();
	for(int i=0; i<nModels-1; ++i) {
		this->answer = this->answer + modelToString(models[i]) + ",";
	}
	this->answer = this->answer + modelToString(models[models.size()-1]) + "]";
}


void DefaultStableSemanticsSolver::isCredulouslyAccepted() {
	std::vector<int> assumps;
	assumps.push_back(varMap.getVar(this->acceptanceQueryArgument));
	solver.computeModel();
	this->answer = solver.hasAModel() ? "YES" : "NO";
}


void DefaultStableSemanticsSolver::isSkepticallyAccepted() {
	std::vector<int> assumps;
	assumps.push_back(-varMap.getVar(this->acceptanceQueryArgument));
	solver.computeModel();
	this->answer = solver.hasAModel() ? "NO" : "YES";
}


DefaultStableSemanticsSolver::~DefaultStableSemanticsSolver() {}

