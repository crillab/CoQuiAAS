/*
 * SatSolver.h
 *
 *  Created on: 21 juil. 2016
 *      Author: lonca
 */

#ifndef SRC_SOLVERS_SATSOLVER_H_
#define SRC_SOLVERS_SATSOLVER_H_


#include <vector>
#include <iostream>
#include <functional>
#include "core/SolverTypes.h"
#include "mtl/Vec.h"


namespace CoQuiAAS {


class SatSolver {

public:

	virtual void addVariables(int nVars) = 0;

	virtual void addVariables(int nVars, bool auxVar) = 0;

	virtual bool addClause(std::vector<int> &clause) = 0;

	virtual int addSelectedClause(std::vector<int> &clause) = 0;

	virtual std::vector<int>& propagatedAtDecisionLvlZero() = 0;

	virtual bool isPropagatedAtDecisionLvlZero(int lit) = 0;

	virtual bool computeModel() = 0;

	virtual bool computeModel(std::vector<int> &assumps) = 0;

	virtual void computeAllModels(std::function<void(std::vector<bool>&)> callback) = 0;

	virtual void computeAllModels(std::function<void(std::vector<bool>&)> callback, std::vector<int> &assumps) = 0;

	virtual bool hasAModel() = 0;

	virtual std::vector<bool>& getModel() = 0;

	virtual std::vector<std::vector<bool> >& getModels() = 0;

	virtual void resetModels() = 0;

	virtual ~SatSolver() {}

protected:

	static void intClauseToBuiltInClause(std::vector<int>& intCl, Minisat::vec<Minisat::Lit>& minisatCl) {
		for(std::vector<int>::iterator it = intCl.begin(); it != intCl.end(); ++it) {
			int lit = *it;
			Minisat::Lit minisatLit = lit > 0 ? Minisat::mkLit(lit-1) : ~Minisat::mkLit(-lit-1);
			minisatCl.push(minisatLit);
		}
	}

	static void printModel(std::vector<bool>& model) {
		int size = (int) model.size();
		for(int i=1; i<=size; ++i) {
			std::cout << (model[i-1] ? i : -i) << " ";
		}
		std::cout << 0 << std::endl;
	}
};


}


#endif /* SRC_SOLVERS_SATSOLVER_H_ */
