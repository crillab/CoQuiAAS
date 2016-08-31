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

	void createAttackersDisjunctionVars(int startId);

	void createCompleteEncodingConstraints(int attackersDisjunctionFirstVar);

	void createStableEncodingConstraints();

	void createStableEncodingConstraints(int attackersDisjunctionFirstVar);

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
