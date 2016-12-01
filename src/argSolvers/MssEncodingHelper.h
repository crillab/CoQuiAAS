/*
 * MssEncodingHelper.h
 *
 *  Created on: 22 août 2016
 *      Author: lonca
 */

#ifndef SRC_SOLVERS_MSSENCODINGHELPER_H_
#define SRC_SOLVERS_MSSENCODINGHELPER_H_


#include "SatEncodingHelper.h"
#include "MssSolver.h"
#include "Attacks.h"
#include "VarMap.h"


namespace CoQuiAAS {


class MssEncodingHelper : public SatEncodingHelper {

public:

	MssEncodingHelper(MssSolver &solver, Attacks& attacks, VarMap& varMap);

	void setMaxExtensionNeeded();

	int setMaxRangeNeeded(int disjVarsStartId);

	std::vector<int> getMaxRangeSelectorsInMss(int selectorsId, std::vector<int>& mss);

	virtual ~MssEncodingHelper();

private:

	MssSolver &solver;
};


}


#endif /* SRC_SOLVERS_MSSENCODINGHELPER_H_ */
