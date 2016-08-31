/*
 * DefaultCompleteSemanticsSolver.cc
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */


#include "DefaultCompleteSemanticsSolver.h"


using namespace CoQuiAAS;


DefaultCompleteSemanticsSolver::DefaultCompleteSemanticsSolver(SatSolver &solver, Attacks &attacks, VarMap &varMap, TaskType taskType)  : SemanticsProblemSolver(attacks, varMap, taskType), solver(solver) {}


void DefaultCompleteSemanticsSolver::init() {
	SatEncodingHelper helper(solver, attacks, varMap);
	int nVars = varMap.nVars();
	helper.createAttackersDisjunctionVars(nVars);
	helper.createCompleteEncodingConstraints(nVars);
}


void DefaultCompleteSemanticsSolver::computeOneExtension() {
	solver.computeModel();
	if(!solver.hasAModel()) {
		this->answer = "NO";
		return;
	}
	std::vector<bool> model = solver.getModel();
	this->answer = modelToString(model);
}


void DefaultCompleteSemanticsSolver::computeAllExtensions() {
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


void DefaultCompleteSemanticsSolver::isCredulouslyAccepted() {
	std::vector<int> assumps;
	assumps.push_back(varMap.getVar(this->acceptanceQueryArgument));
	solver.computeModel(assumps);
	this->answer = solver.hasAModel() ? "YES" : "NO";
}


void DefaultCompleteSemanticsSolver::isSkepticallyAccepted() {
	std::vector<int> assumps;
	assumps.push_back(-varMap.getVar(this->acceptanceQueryArgument));
	solver.computeModel(assumps);
	this->answer = solver.hasAModel() ? "NO" : "YES";
}


DefaultCompleteSemanticsSolver::~DefaultCompleteSemanticsSolver() {
	// TODO Auto-generated destructor stub
}

