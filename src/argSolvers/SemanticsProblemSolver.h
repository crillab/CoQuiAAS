/*
 * SemanticsProblemSolver.h
 *
 *  Created on: 22 juil. 2016
 *      Author: lonca
 */


#ifndef SRC_SOLVERS_SEMANTICSPROBLEMSOLVER_H_
#define SRC_SOLVERS_SEMANTICSPROBLEMSOLVER_H_

#include <cstdlib>
#include <iostream>

#include "Types.h"
#include "Attacks.h"
#include "VarMap.h"
#include "SolverOutputFormatter.h"


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

	inline void setDynStep(int step) {this->dynStep = step;}

protected:

	TaskType taskType;

	Attacks &attacks;

	VarMap &varMap;

	SolverOutputFormatter &formatter;

	std::string acceptanceQueryArgument;

	int dynStep = -1;
};


}


#endif /* SRC_SOLVERS_SEMANTICSPROBLEMSOLVER_H_ */
