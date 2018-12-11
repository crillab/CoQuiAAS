/*
 * SatEncodingHelper.h
 *
 *  Created on: 21 juil. 2016
 *      Author: lonca
 */

#ifndef SRC_SOLVERS_SATENCODINGHELPER_H_
#define SRC_SOLVERS_SATENCODINGHELPER_H_


#include <memory>

#include "SatSolver.h"
#include "Attacks.h"
#include "VarMap.h"


namespace CoQuiAAS {


class SatEncodingHelper {

public:

	SatEncodingHelper(std::shared_ptr<SatSolver> solver, Attacks& attacks, VarMap& varMap);

	int reserveVars(int n);

	int reserveDisjunctionVars();

	void createAttackersDisjunctionVars(int startId);

	void createConflictFreenessEncodingConstraints(int startId);

	void createCompleteEncodingConstraints(int startId);

	void createStableEncodingConstraints();

	void createStableEncodingConstraints(int startId);

	std::vector<int> dynAssumps(int step);

	virtual ~SatEncodingHelper();

private:

	std::shared_ptr<SatSolver> solver;

protected:

	void reserveDynVars();

	Attacks &attacks;

	VarMap &varMap;

	// (argFrom, argTo, fromReplInEnc, notAttackedReplInEnc, assump)
	std::vector<std::tuple<int, int, int, int, int> > dynVars;

	int nbVars;
};


}


#endif /* SRC_SOLVERS_SATENCODINGHELPER_H_ */
