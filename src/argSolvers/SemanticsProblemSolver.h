/*
 * SemanticsProblemSolver.h
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */


#ifndef SRC_SOLVERS_SEMANTICSPROBLEMSOLVER_H_
#define SRC_SOLVERS_SEMANTICSPROBLEMSOLVER_H_

#include <ctime>
#include <cstdlib>
#include <iostream>

#include "Types.h"
#include "Attacks.h"
#include "VarMap.h"
#include "SolverOutputFormatter.h"
#include "Logger.h"


namespace CoQuiAAS {


class SemanticsProblemSolver {

public:

	SemanticsProblemSolver(Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter);

	virtual void init() = 0;

	void compute();

	virtual void computeOneExtension() = 0;

	virtual void computeAllExtensions() = 0;

	virtual void setAcceptanceQueryArgument(std::string arg);

	virtual void isCredulouslyAccepted() = 0;

	virtual void isSkepticallyAccepted() = 0;

	virtual ~SemanticsProblemSolver();

	inline Attacks& getAttacks() {return attacks;}

	inline VarMap& getVarMap() {return varMap;}

	inline TaskType getTaskType() {return taskType;}

	inline SolverOutputFormatter& getFormatter() {return formatter;}

	inline void setDynStep(int step) {
		this->dynStep = step;
	}

protected:

	TaskType taskType;

	Attacks &attacks;

	VarMap &varMap;

	SolverOutputFormatter &formatter;

	std::string acceptanceQueryArgument = "";

	int dynStep = -1;

	inline void logSingleExtTime(clock_t startTime) {
		Logger::getInstance()->info("time spent to compute one extension: %.3fs", (double)(clock()-startTime)/CLOCKS_PER_SEC);
	}

	inline void logOneExtTime(clock_t startTime, int extIndex) {
		Logger::getInstance()->info("time spent to extension %d: %.3fs", extIndex, (double)(clock()-startTime)/CLOCKS_PER_SEC);
	}

	inline void logNoMoreExts(clock_t startTime) {
		Logger::getInstance()->info("time spent to check there is no more extension: %.3fs", (double)(clock()-startTime)/CLOCKS_PER_SEC);
	}

	inline void logAllExtsTime(clock_t startTime) {
		Logger::getInstance()->info("time spent to compute all extensions: %.3fs", (double)(clock()-startTime)/CLOCKS_PER_SEC);
	}

	inline void logAcceptanceCheckingTime(clock_t startTime) {
		Logger::getInstance()->info("time spent to check argument acceptance: %.3fs", (double)(clock()-startTime)/CLOCKS_PER_SEC);
	}
};


}


#endif /* SRC_SOLVERS_SEMANTICSPROBLEMSOLVER_H_ */
