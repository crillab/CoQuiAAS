/*
 * MultiSolver.cc
 *
 *  Created on: 23 août 2016
 *      Author: lonca
 */

#include "MultiSolver.h"


using namespace CoQuiAAS;


MultiSolver::MultiSolver(Attacks &attacks, VarMap &varMap, TaskType taskType, std::vector<SemanticsProblemSolver*> &solvers) : SemanticsProblemSolver(attacks, varMap, taskType), solvers(solvers) {
	answerFound = false;
}


void MultiSolver::init() {
	for(int i=0; i< (signed) solvers.size(); ++i) {
		solvers[i]->init();
	}
}


void MultiSolver::computeOneExtension() {
	launchAllTasksAndLock(TASK_ONE_EXT);
}


void MultiSolver::computeAllExtensions() {
	launchAllTasksAndLock(TASK_ALL_EXTS);
}


void MultiSolver::isCredulouslyAccepted() {
	launchAllTasksAndLock(TASK_CRED_INF);
}


void MultiSolver::isSkepticallyAccepted() {
	launchAllTasksAndLock(TASK_SKEP_INF);
}


void MultiSolver::launchAllTasksAndLock(TaskType task) {
	sem_init(&thlock, 0, 0);
	sem_init(&ulock, 0, 1);
	for(int i=0; i< (signed) solvers.size(); ++i) {
		char *thData = (char*) malloc(2*sizeof(SemanticsProblemSolver*) + sizeof(int) + sizeof(int));
		SemanticsProblemSolver *thisptr = this;
		memcpy(thData, &thisptr, sizeof(SemanticsProblemSolver*));
		SemanticsProblemSolver *solverptr = solvers[i];
		memcpy(thData+sizeof(SemanticsProblemSolver*), &solverptr, sizeof(SemanticsProblemSolver*));
		memcpy(thData+sizeof(SemanticsProblemSolver*)+sizeof(SemanticsProblemSolver*), &task, sizeof(int));
		memcpy(thData+sizeof(SemanticsProblemSolver*)+sizeof(SemanticsProblemSolver*)+sizeof(int), &i, sizeof(int));
		pthread_t th;
		pthread_create(&th, NULL, launchConcurrentTask, thData);
		threadIds.push_back(th);
		pthread_detach(th);
	}
	for(int i=0; i<(signed) solvers.size(); ++i) sem_post(&thlock);
	sem_init(&plock, 0, 0);
	sem_wait(&plock);
}


void *CoQuiAAS::launchConcurrentTask(void *data) {
	MultiSolver *multisolver = NULL;
	memcpy(&multisolver, data, sizeof(SemanticsProblemSolver*));
	SemanticsProblemSolver *solver = NULL;
	memcpy(&solver, ((char*)data) + sizeof(SemanticsProblemSolver*), sizeof(SemanticsProblemSolver*));
	TaskType task;
	memcpy(&task, ((char*) data) + 2*sizeof(SemanticsProblemSolver*), sizeof(int));
	int solverIndex;
	memcpy(&solverIndex, ((char*) data) + 2*sizeof(SemanticsProblemSolver*) + sizeof(int), sizeof(int));
	free(data);
	sem_wait(&multisolver->thlock);
	switch(task) {
	case TASK_ONE_EXT:
		solver->computeOneExtension();
		break;
	case TASK_ALL_EXTS:
		solver->computeAllExtensions();
		break;
	case TASK_CRED_INF:
		solver->isCredulouslyAccepted();
		break;
	case TASK_SKEP_INF:
		solver->isSkepticallyAccepted();
		break;
	default:
		std::cerr << "wrong task type ; exiting" << std::endl;
		exit(2);
	}
	multisolver->unlock(solverIndex);
	return NULL;
}


void MultiSolver::unlock(int requiredBy) {
	sem_wait(&ulock);
	if(!answerFound) {
		answer = solvers[requiredBy]->answerToString();
		for(int i=0; i<(signed) threadIds.size(); ++i) {
			if(i == requiredBy) continue;
			pthread_cancel(threadIds[i]);
		}
	}
	answerFound = true;
	sem_post(&plock);
}


MultiSolver::~MultiSolver() {
	sem_destroy(&ulock);
	sem_destroy(&plock);
	sem_destroy(&thlock);
}
