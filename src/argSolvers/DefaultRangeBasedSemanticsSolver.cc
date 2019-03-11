#include "DefaultRangeBasedSemanticsSolver.h"


using namespace CoQuiAAS;


DefaultRangeBasedSemanticsSolver::DefaultRangeBasedSemanticsSolver(std::shared_ptr<MssSolver> solver, Attacks &attacks, VarMap &varMap, TaskType taskType, SolverOutputFormatter &formatter):
	SemanticsProblemSolver(attacks, varMap, taskType, formatter), solver(solver) {
	this->solver->setBlockingClauseFunction([this](std::vector<bool>& model) -> std::vector<int> {
		std::vector<int> intCl;
		for(int i=0; i<this->problemReducer->getReducedMap()->nVars(); ++i) {
			if(model[i]) intCl.push_back(-i-1);
		}
		return intCl;
	});
}


void DefaultRangeBasedSemanticsSolver::computeOneExtension() {
	clock_t startTime = clock();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	solver->computeMss(dynAssumps);
	if(!solver->hasAMss()) {
		std::cerr << "problem is UNSAT (although is cannot)" << std::endl;
		return;
	}
	this->formatter.writeSingleExtension(solver->getModel());
	logSingleExtTime(startTime);
}


void DefaultRangeBasedSemanticsSolver::computeAllExtensions() {
	clock_t globalStartTime = clock();
	this->formatter.writeExtensionListBegin();
	int extIndex = 1;
	clock_t startTime = clock();
	std::vector<int>& propagated = solver->propagatedAtDecisionLvlZero();
	std::vector<bool> grExt = SatSolver::toBoolModel(propagated, this->problemReducer->getReducedMap()->nVars());
	std::vector<std::vector<bool> > models = computeAllExtensions([this, &extIndex, &startTime, grExt](std::vector<bool>& model) {
		if(extIndex == 1) {
			bool isGrounded = true;
			for(unsigned int i=0; i<this->problemReducer->getReducedMap()->nVars(); ++i) {
				if(grExt[i] != model[i]) {
					isGrounded = false;
					break;
				}
			}
			if(isGrounded) {
				this->stopEnum = true;
				solver->stopMssEnum();
			}
		}
		this->formatter.writeExtensionListElmt(model, extIndex == 1);
		extIndex++;
		startTime = clock();
	});
	logNoMoreExts(startTime);
	this->formatter.writeExtensionListEnd();
	logAllExtsTime(globalStartTime);
}


std::vector<std::vector<bool>> DefaultRangeBasedSemanticsSolver::computeAllExtensions(std::function<void(std::vector<bool>&)> callback) {
	this->stopEnum = false;
	std::vector<std::vector<int>> msses;
	std::vector<std::vector<bool>> oldModels;
	std::vector<std::vector<bool>> extModels;
	std::shared_ptr<VarMap> reducedVM = this->problemReducer->getReducedMap();
	std::vector<int> dynAssumps = this->helper->dynAssumps(this->dynStep);
	solver->computeAllMss([this, callback, &msses, &oldModels, reducedVM](std::vector<int>& mss, std::vector<bool>& model){
		msses.push_back(mss);
		oldModels.push_back(model);
		if(callback != NULL) callback(model);
		bool emptyExt = true;
		for(unsigned int i=0; i<reducedVM->nVars() && emptyExt; ++i) emptyExt &= !model[i];
		if(this->stopEnum) {
			Logger::getInstance()->trace("Range-based solver was required to stop enumeration process");
			solver->stopMssEnum();
		}
		if(emptyExt) {
			Logger::getInstance()->trace("Found an empty maximal MSS; stopping enumeration process");
			solver->stopMssEnum();
			this->stopEnum = true;
		}
	}, dynAssumps);
	solver->resetAllMss();
	solver->resetModels();
	Logger::getInstance()->trace("MSS enumerator found %d MSSes for range-based solver", msses.size());
	if(this->stopEnum) return extModels;
	std::vector<std::string> varNames = reducedVM->getNames();
	std::vector<int> selectors;
	for(unsigned int i=0; i<varNames.size(); ++i) {
		std::vector<int> cl;
		std::string varName = varNames[i];
		cl.push_back(reducedVM->getVar(varName));
		cl.push_back(this->helper->getDisjunctionVar(varName));
		selectors.push_back(solver->addSelectedClause(cl));
	}
	int oldExtModelsSize = 0;
	for(int i=0; i<(signed)msses.size(); ++i) {
		extModels.push_back(oldModels[i]);
		std::vector<int> assumps;
		for(unsigned int j=0; j<selectors.size(); ++j) {
			assumps.push_back(-selectors[j]);
		}
		for(int j=0; j<(signed)msses[i].size(); ++j) {
			assumps[msses[i][j]-1] = selectors[msses[i][j]-1];
		}
		for(unsigned int j=0; j<varNames.size(); ++j) {
			if(assumps[j] == -selectors[j]) {
				std::string varName = varNames[j];
				assumps.push_back(-reducedVM->getVar(varName));
				assumps.push_back(-this->helper->getDisjunctionVar(varName));
			}
		}
		std::vector<int> cl;
		for(int j=0; j<reducedVM->nVars(); ++j) {
			if(!oldModels[i][j]) cl.push_back(j+1);
		}
		for(unsigned int j=0; j<dynAssumps.size(); ++j) assumps.push_back(dynAssumps[j]);
		auto blockingSel = solver->addSelectedClause(cl);
		cl.clear();
		cl.push_back(-blockingSel);
		assumps.push_back(blockingSel);
		for(int j=0; j<reducedVM->nVars(); ++j) {
			if(oldModels[i][j]) cl.push_back(-j-1);
		}
		solver->addClause(cl);
		solver->computeAllModels([this,callback,&extModels](std::vector<bool>& model){
			extModels.push_back(model);
			if(callback != NULL) callback(model);
			if(this->stopEnum) {
				solver->stopMssEnum();
				Logger::getInstance()->trace("Range-based solver was required to stop enumeration process");
			}
		}, assumps);
		std::vector<int> unitCl;
		unitCl.push_back(-blockingSel);
		solver->addClause(unitCl);
		solver->resetModels();
		Logger::getInstance()->trace("Range-based solver found %d extension models for MSS %d", extModels.size()-oldExtModelsSize, i+1);
		oldExtModelsSize = extModels.size();
		if(this->stopEnum) break;
	}
	for(int i=0; i<(signed)selectors.size(); ++i) {
		std::vector<int> cl;
		cl.push_back(-selectors[i]);
		solver->addClause(cl);
	}
	return extModels;
}


void DefaultRangeBasedSemanticsSolver::isCredulouslyAccepted() {
	clock_t startTime = clock();
	bool status = false;
	int arg = this->problemReducer->getReducedMap()->getVar(this->problemReducer->translateAcceptanceQueryArgument(this->acceptanceQueryArgument));
	std::vector<int>& propagated = solver->propagatedAtDecisionLvlZero();
	bool isPropagated = false;
	bool propagatedValue = false;
	for(unsigned int i=0; i<propagated.size(); ++i) {
		if(propagated[i] == arg) {
			isPropagated = true;
			propagatedValue = true;
			break;
		} else if(propagated[i] == -arg) {
			isPropagated = true;
			propagatedValue = false;
			break;
		}
	}
	if(isPropagated) {
		status = propagatedValue;
	} else {
		std::vector<std::vector<bool> > models = computeAllExtensions([this,arg,&status](std::vector<bool>& model){
			if(model[arg-1]) {
				status = true;
				Logger::getInstance()->trace("Credulous acceptance was demonstrated; stopping MSS enumeration for range-based solver");
				this->stopEnum = true;
			}
		});
	}
	this->formatter.writeArgAcceptance(status);
	logAcceptanceCheckingTime(startTime);
}


void DefaultRangeBasedSemanticsSolver::isSkepticallyAccepted() {
	clock_t startTime = clock();
	bool status = true;
	int arg = this->problemReducer->getReducedMap()->getVar(this->problemReducer->translateAcceptanceQueryArgument(this->acceptanceQueryArgument));
	std::vector<int>& propagated = solver->propagatedAtDecisionLvlZero();
	bool isPropagated = false;
	bool propagatedValue = false;
	for(unsigned int i=0; i<propagated.size(); ++i) {
		if(propagated[i] == arg) {
			isPropagated = true;
			propagatedValue = true;
			break;
		} else if(propagated[i] == -arg) {
			isPropagated = true;
			propagatedValue = false;
			break;
		}
	}
	if(isPropagated) {
		status = propagatedValue;
	} else {
		std::vector<std::vector<bool> > models = computeAllExtensions([this,arg,&status](std::vector<bool>& model){
			if(!model[arg-1]) {
				status = false;
				Logger::getInstance()->trace("Skeptical non-acceptance was demonstrated; stopping MSS enumeration for range-based solver");
				this->stopEnum = true;
			}
		});
	}
	this->formatter.writeArgAcceptance(status);
	logAcceptanceCheckingTime(startTime);
}


DefaultRangeBasedSemanticsSolver::~DefaultRangeBasedSemanticsSolver() {}


