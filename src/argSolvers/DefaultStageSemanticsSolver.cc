/*
 * DefaultStageSemanticsSolver.cc
 *
 *  Created on: 5 déc. 2016
 *      Author: lonca
 */


#include "DefaultStageSemanticsSolver.h"


using namespace CoQuiAAS;


DefaultStageSemanticsSolver::DefaultStageSemanticsSolver(std::shared_ptr<MssSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter):
	DefaultRangeBasedSemanticsSolver(solver, attacks, varMap, taskType, formatter){}


void DefaultStageSemanticsSolver::init() {
	this->helper = new MssEncodingHelper(solver, attacks, varMap);
	int disjId = this->helper->reserveDisjunctionVars();
	this->helper->setMaxRangeNeeded(disjId);
	this->helper->createAttackersDisjunctionVars(disjId);
	this->helper->createConflictFreenessEncodingConstraints(disjId);
}
