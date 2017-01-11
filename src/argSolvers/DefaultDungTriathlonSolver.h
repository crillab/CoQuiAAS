/*
 * DefaultDungTriathlonSolver.h
 *
 *  Created on: 11 janv. 2017
 *      Author: lonca
 */

#ifndef SRC_ARGSOLVERS_DEFAULTDUNGTRIATHLONSOLVER_H_
#define SRC_ARGSOLVERS_DEFAULTDUNGTRIATHLONSOLVER_H_


#include "SemanticsProblemSolver.h"
#include "MssSolver.h"
#include "MssEncodingHelper.h"
#include "ExtensionUtils.h"


namespace CoQuiAAS {

class DefaultDungTriathlonSolver : public SemanticsProblemSolver {

public:

	DefaultDungTriathlonSolver(MssSolver &solver, Attacks &attacks, VarMap &varMap);

	void init();

	void computeOneExtension();

	void computeAllExtensions();

	void isCredulouslyAccepted();

	void isSkepticallyAccepted();

	virtual ~DefaultDungTriathlonSolver();

private:

	void addGroundedExtensions();

	void addPreferredExtensions();

	void addStableExtensions();

	MssSolver &solver;
};


} /* namespace CoQuiAAS */

#endif /* SRC_ARGSOLVERS_DEFAULTDUNGTRIATHLONSOLVER_H_ */
