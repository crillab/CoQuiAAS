/*
 * MultiSolver.h
 *
 *  Created on: 23 août 2016
 *      Author: lonca
 */

#ifndef SRC_ARGSOLVERS_MULTISOLVER_H_
#define SRC_ARGSOLVERS_MULTISOLVER_H_


#include "SemanticsProblemSolver.h"
#include "Types.h"

#include <cstring>
#include <semaphore.h>


namespace CoQuiAAS {


extern void *launchConcurrentTask(void *data);


class MultiSolver : public SemanticsProblemSolver {

public:

	MultiSolver(Attacks &attacks, VarMap &varMap, TaskType taskType, std::vector<SemanticsProblemSolver*> &solvers);

	virtual void init();

	virtual void computeOneExtension();

	virtual void computeAllExtensions();

	virtual void isCredulouslyAccepted();

	virtual void isSkepticallyAccepted();

	virtual ~MultiSolver();

	void unlock(int requiredBy);

	friend void *launchConcurrentTask(void *data);

private:

	bool answerFound;

	void launchAllTasksAndLock(TaskType task);

	std::vector<SemanticsProblemSolver*> &solvers;

	std::vector<pthread_t> threadIds;

	sem_t thlock;

	sem_t plock;

	sem_t ulock;

};


}


#endif /* SRC_ARGSOLVERS_MULTISOLVER_H_ */
