/*
 * SemanticsProblemSolver.cc
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */


#include "SemanticsProblemSolver.h"


using namespace CoQuiAAS;


SemanticsProblemSolver::SemanticsProblemSolver(Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter) : attacks(attacks), varMap(varMap), formatter(formatter) {
	this->taskType = taskType;
}


void SemanticsProblemSolver::compute() {
	switch(this->taskType) {
	case TASK_ONE_EXT:
		computeOneExtension();
		break;
	case TASK_ALL_EXTS:
		computeAllExtensions();
		break;
	case TASK_CRED_INF:
		isCredulouslyAccepted();
		break;
	case TASK_SKEP_INF:
		isSkepticallyAccepted();
		break;
	default:
		std::cerr << "Unknown task" << std::endl;
		exit(1);
	}
}


void SemanticsProblemSolver::setAcceptanceQueryArgument(std::string arg) {
	this->acceptanceQueryArgument = arg;
}


SemanticsProblemSolver::~SemanticsProblemSolver() {}

