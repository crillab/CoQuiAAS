/*
 * MaxSatSolver.h
 *
 *  Created on: 5 déc. 2016
 *      Author: lonca
 */

#ifndef SRC_SATSOLVERS_MAXSATSOLVER_H_
#define SRC_SATSOLVERS_MAXSATSOLVER_H_

#include "SatSolver.h"


namespace CoQuiAAS {


class MaxSatSolver : public SatSolver {

public:

	virtual void addSoftClause(std::vector<int> &clause) = 0;

	virtual bool computeMaxSat() = 0;

	virtual bool computeMaxSat(std::vector<int> &assumps) = 0;

	virtual void computeAllMaxSat() = 0;

	virtual void computeAllMaxSat(std::vector<int> &assumps) = 0;

	virtual std::vector<bool>& getMaxSat() = 0;

	virtual std::vector<std::vector<bool> >& getAllMaxSat() = 0;

	virtual int getOptValue() = 0;
};


}


#endif /* SRC_SATSOLVERS_MAXSATSOLVER_H_ */
