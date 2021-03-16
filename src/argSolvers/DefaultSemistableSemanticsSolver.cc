/*
 * DefaultSemistableSemanticsSolver.cc
 *
 *  Created on: 5 déc. 2016
 *      Author: lonca
 */


#include "DefaultSemistableSemanticsSolver.h"


using namespace CoQuiAAS;


DefaultSemistableSemanticsSolver::DefaultSemistableSemanticsSolver(std::shared_ptr<MssSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter):
	DefaultRangeBasedSemanticsSolver(solver, attacks, varMap, taskType, formatter){}


void DefaultSemistableSemanticsSolver::init() {
	this->problemReducer = std::make_unique<CompleteEncodingSatProblemReducer>(varMap, attacks);
	this->problemReducer->search();
	VarMap &reducedMap = *this->problemReducer->getReducedMap().get();
	this->formatter.setVarMap(reducedMap);
	this->helper = new MssEncodingHelper(solver, attacks, reducedMap);
	int disjId = this->helper->reserveDisjunctionVars();
	this->helper->setMaxRangeNeeded(disjId);
	this->helper->createAttackersDisjunctionVars(disjId);
	this->helper->createCompleteEncodingConstraints(disjId);
}
