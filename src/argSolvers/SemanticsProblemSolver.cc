/*
 * SemanticsProblemSolver.cc
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */


#include "SemanticsProblemSolver.h"


SemanticsProblemSolver::SemanticsProblemSolver(Attacks &attacks, VarMap &varMap, TaskType taskType) : attacks(attacks), varMap(varMap) {
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


std::string SemanticsProblemSolver::modelToString(vector<bool>& model) {
  string res;

  bool noneFoundYet = true;
  res = "[";
  int nVars = varMap.nVars();
  for (int i = 0; i < nVars; i++) {
    if (model[i]) {
      if(noneFoundYet) noneFoundYet=false; else res=res + ",";
      res = res + varMap.getName(i+1);
    }
  }
  res = res + "]";
  return res;
}


std::string SemanticsProblemSolver::modelToString(vector<int>& model) {
  string res;

  bool noneFoundYet = true;
  res = "[";
  int size = (signed) model.size();
  for(int i=0; i<size; ++i) {
	  if(model[i] < 0) continue;
	  if(noneFoundYet) {
		  noneFoundYet = false;
	  } else {
		  res = res + ",";
	  }
	  res = res + varMap.getName(model[i]);
  }
  res = res + "]";
  return res;
}


std::string SemanticsProblemSolver::answerToString() {
	return this->answer;
}


SemanticsProblemSolver::~SemanticsProblemSolver() {}

