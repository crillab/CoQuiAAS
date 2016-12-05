/*
 * SatEncodingHelper.h
 *
 *  Created on: 21 juil. 2016
 *      Author: lonca
 */

#ifndef SRC_SOLVERS_SATENCODINGHELPER_H_
#define SRC_SOLVERS_SATENCODINGHELPER_H_


#include "SatSolver.h"
#include "Attacks.h"
#include "VarMap.h"


namespace CoQuiAAS {


class SatEncodingHelper {

public:

	SatEncodingHelper(SatSolver &solver, Attacks& attacks, VarMap& varMap);

	int reserveVars(int n);

	int reserveDisjunctionVars();

	void createAttackersDisjunctionVars(int startId);

	void createConflictFreenessEncodingConstraints(int startId);

	void createCompleteEncodingConstraints(int startId);

	void createStableEncodingConstraints();

	void createStableEncodingConstraints(int startId);

	virtual ~SatEncodingHelper();

private:

	SatSolver &solver;

protected:

	Attacks &attacks;

	VarMap &varMap;

	int nbVars;
};


}


#endif /* SRC_SOLVERS_SATENCODINGHELPER_H_ */
