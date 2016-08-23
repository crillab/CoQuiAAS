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


class SemanticsProblemSolver {

public:

	SemanticsProblemSolver(Attacks &attacks, VarMap &varMap, TaskType taskType);

	virtual void init() = 0;

	void compute();

	virtual void computeOneExtension() = 0;

	virtual void computeAllExtensions() = 0;

	void setAcceptanceQueryArgument(std::string arg);

	virtual void isCredulouslyAccepted() = 0;

	virtual void isSkepticallyAccepted() = 0;

	std::string answerToString();

	virtual ~SemanticsProblemSolver();

protected:

	TaskType taskType;

	Attacks &attacks;

	VarMap &varMap;

	std::string acceptanceQueryArgument;

	std::string answer;

	std::string modelToString(vector<bool>& model);

	std::string modelToString(vector<int>& model);
};

#endif /* SRC_SOLVERS_SEMANTICSPROBLEMSOLVER_H_ */
