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


class MssEncodingHelper : public SatEncodingHelper {

public:

	MssEncodingHelper(MssSolver &solver, Attacks& attacks, VarMap& varMap);

	void setMaxExtensionNeeded();

	virtual ~MssEncodingHelper();

private:

	MssSolver &solver;
};

#endif /* SRC_SOLVERS_MSSENCODINGHELPER_H_ */
