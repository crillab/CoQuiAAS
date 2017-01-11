/*
 * GraphBasedGroundedSemanticsSolver.h
 *
 *  Created on: 11 janv. 2017
 *      Author: lonca
 */

#ifndef SRC_ARGSOLVERS_GRAPHBASEDGROUNDEDSEMANTICSSOLVER_H_
#define SRC_ARGSOLVERS_GRAPHBASEDGROUNDEDSEMANTICSSOLVER_H_


#include "SemanticsProblemSolver.h"
#include "ExtensionUtils.h"


namespace CoQuiAAS {

class GraphBasedGroundedSemanticsSolver : public SemanticsProblemSolver {

public:

	GraphBasedGroundedSemanticsSolver(Attacks &attacks, VarMap &varMap, TaskType taskType);

	void init();

	void computeOneExtension();

	void computeAllExtensions();

	void isCredulouslyAccepted();

	void isSkepticallyAccepted();

	virtual ~GraphBasedGroundedSemanticsSolver();
};

} /* namespace CoQuiAAS */

#endif /* SRC_ARGSOLVERS_GRAPHBASEDGROUNDEDSEMANTICSSOLVER_H_ */
