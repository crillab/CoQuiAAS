/*
 * MaxSatEncodingHelper.h
 *
 *  Created on: 5 déc. 2016
 *      Author: lonca
 */

#ifndef SRC_ARGSOLVERS_MAXSATENCODINGHELPER_H_
#define SRC_ARGSOLVERS_MAXSATENCODINGHELPER_H_


#include <memory>

#include "SatEncodingHelper.h"
#include "MaxSatSolver.h"
#include "Attacks.h"
#include "VarMap.h"


namespace CoQuiAAS {


class MaxSatEncodingHelper : public SatEncodingHelper {

public:

	MaxSatEncodingHelper(std::shared_ptr<MaxSatSolver> solver, Attacks& attacks, VarMap& varMap);

	void setMaxRangeNeeded(int disjVarsStartId);

	void setMaxExtensionNeeded(int disjVarsStartId);

	virtual ~MaxSatEncodingHelper();

private:

	std::shared_ptr<MaxSatSolver> solver;
};


}


#endif /* SRC_ARGSOLVERS_MAXSATENCODINGHELPER_H_ */
