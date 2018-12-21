#ifndef __SOLVER_FACTORY_H__
#define __SOLVER_FACTORY_H__

#include <memory>
#include <algorithm>
#include <string>
#include <map>
#include <initializer_list>

#include "DefaultStableSemanticsSolver.h"
#include "DefaultStageSemanticsSolver.h"
#include "DefaultSemistableSemanticsSolver.h"
#include "DefaultCompleteSemanticsSolver.h"
#include "DefaultGroundedSemanticsSolver.h"
#include "GraphBasedGroundedSemanticsSolver.h"
#include "DefaultPreferredSemanticsSolver.h"
#include "DefaultIdealSemanticsSolver.h"
#include "DefaultDungTriathlonSolver.h"
#include "BuiltInSatSolverNG.h"
#include "ExternalSatSolver.h"
#include "BuiltInMssSolverNG.h"
#include "SolverOutputFormatter.h"
#include "DynamicSemanticsSolverDecorator.h"


namespace CoQuiAAS {


/**
 * \class SolverFactory
 * \brief provides utility methods to build problem solvers given the task and the semantic that are required ; also provide some utility methods for semantic and task recognization
 */
class SolverFactory {

public:

	/**
	 * \fn getTaskType(str)
	 * \brief try to recognize a task in a string
	 * \param str : the string to parse
	 */
	static TaskType getTaskType(std::string str) {
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
		if(!str.compare("D3")) return TASK_TRIATHLON;
		auto indexOf = str.find("-");
		if(indexOf == std::string::npos) return TASK_UNDEFINED;
		str = str.substr(0,indexOf);
		if(!str.compare("SE")) return TASK_ONE_EXT;
		if(!str.compare("EE")) return TASK_ALL_EXTS;
		if(!str.compare("DC")) return TASK_CRED_INF;
		if(!str.compare("DS")) return TASK_SKEP_INF;
		return TASK_UNDEFINED;
	}

	static Semantics getSemantics(std::string str) {
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
		if(!str.compare("D3")) return Semantics(SEM_TRIATHLON, false);
		auto indexOf = str.find("-");
		if(indexOf == std::string::npos) return Semantics(SEM_UNDEFINED, false);
		str = str.substr(indexOf+1);
		indexOf = str.find("-");
		bool isDyn = false;
		if(indexOf != std::string::npos && !str.substr(indexOf+1).compare("D")) {
			isDyn = true;
			str = str.substr(0, indexOf);
		}
		if(!str.compare("ST")) return Semantics(SEM_STABLE, isDyn);
		if(!str.compare("CO")) return Semantics(SEM_COMPLETE, isDyn);
		if(!str.compare("GR")) return Semantics(SEM_GROUNDED, isDyn);
		if(!str.compare("PR")) return Semantics(SEM_PREFERRED, isDyn);
		if(!str.compare("SST")) return Semantics(SEM_SEMISTABLE, isDyn);
		if(!str.compare("STG")) return Semantics(SEM_STAGE, isDyn);
		if(!str.compare("ID")) return Semantics(SEM_IDEAL, isDyn);
		return Semantics(SEM_UNDEFINED, false);
	}

	static std::shared_ptr<SatSolver> createSatSolver(std::map<std::string,std::string>& additionalParams) {
		if(additionalParams.find("-externalSatSolver") != additionalParams.end()) {
			return std::shared_ptr<SatSolver>(std::make_shared<ExternalSatSolver>(additionalParams["-externalSatSolver"]));
		}
		return std::shared_ptr<SatSolver>(std::make_shared<BuiltInSatSolverNG>());
	}

	static std::shared_ptr<MssSolver> createMssSolver(std::map<std::string,std::string>& additionalParams) {
		return std::shared_ptr<MssSolver>(std::make_shared<BuiltInMssSolverNG>());
	}

	static std::unique_ptr<SemanticsProblemSolver> groundedSolver(TaskType task, std::map<std::string,std::string>& additionalParams, Attacks &attacks, VarMap &varMap, SolverOutputFormatter &outputFormatter) {
		if(additionalParams.find("--graphBased") != additionalParams.end()) {
			return std::unique_ptr<SemanticsProblemSolver>(new DynamicSemanticsSolverDecorator<GraphBasedGroundedSemanticsSolver>(*new GraphBasedGroundedSemanticsSolver(attacks, varMap, task, outputFormatter)));
		}
		return std::unique_ptr<SemanticsProblemSolver>(new DynamicSemanticsSolverDecorator<DefaultGroundedSemanticsSolver>(*new DefaultGroundedSemanticsSolver(createSatSolver(additionalParams), attacks, varMap, task, outputFormatter)));
	}

	static std::unique_ptr<SemanticsProblemSolver> undecoratedGroundedSolver(TaskType task, std::map<std::string,std::string>& additionalParams, Attacks &attacks, VarMap &varMap, SolverOutputFormatter &outputFormatter) {
		if(additionalParams.find("--graphBased") != additionalParams.end()) {
			return std::unique_ptr<SemanticsProblemSolver>(new GraphBasedGroundedSemanticsSolver(attacks, varMap, task, outputFormatter));
		}
		return std::unique_ptr<SemanticsProblemSolver>(new DefaultGroundedSemanticsSolver(createSatSolver(additionalParams), attacks, varMap, task, outputFormatter));
	}

	static std::unique_ptr<SemanticsProblemSolver> getProblemInstance(Semantics semantic, TaskType task, std::map<std::string,std::string>& additionalParams, Attacks &attacks, VarMap &varMap, SolverOutputFormatter &outputFormatter) {
		if(task == TASK_UNDEFINED) return nullptr;
		if(!semantic.isDynamic()) {
			return getUndecoratedProblemInstance(semantic, task, additionalParams, attacks, varMap, outputFormatter);
		}
		switch(semantic.getName()) {
		case SEM_STABLE:
			return std::unique_ptr<SemanticsProblemSolver>(new DynamicSemanticsSolverDecorator<DefaultStableSemanticsSolver>(*new DefaultStableSemanticsSolver(createSatSolver(additionalParams), attacks, varMap, task, outputFormatter)));
		case SEM_COMPLETE:
			return std::unique_ptr<SemanticsProblemSolver>(new DynamicSemanticsSolverDecorator<DefaultCompleteSemanticsSolver>(*new DefaultCompleteSemanticsSolver(createSatSolver(additionalParams), attacks, varMap, task, outputFormatter)));
		case SEM_GROUNDED:
			return groundedSolver(task, additionalParams, attacks, varMap, outputFormatter);
		case SEM_PREFERRED:
			return std::unique_ptr<SemanticsProblemSolver>(new DynamicSemanticsSolverDecorator<DefaultPreferredSemanticsSolver>(*new DefaultPreferredSemanticsSolver(createMssSolver(additionalParams), attacks, varMap, task, outputFormatter)));
		case SEM_SEMISTABLE:
			return std::unique_ptr<SemanticsProblemSolver>(new DynamicSemanticsSolverDecorator<DefaultSemistableSemanticsSolver>(*new DefaultSemistableSemanticsSolver(createMssSolver(additionalParams), attacks, varMap, task, outputFormatter)));
		case SEM_STAGE:
			return std::unique_ptr<SemanticsProblemSolver>(new DynamicSemanticsSolverDecorator<DefaultStageSemanticsSolver>(*new DefaultStageSemanticsSolver(createMssSolver(additionalParams), attacks, varMap, task, outputFormatter)));
		case SEM_IDEAL:
			return std::unique_ptr<SemanticsProblemSolver>(new DynamicSemanticsSolverDecorator<DefaultIdealSemanticsSolver>(*new DefaultIdealSemanticsSolver(createMssSolver(additionalParams), attacks, varMap, task, outputFormatter)));
		case SEM_TRIATHLON:
			return std::unique_ptr<SemanticsProblemSolver>(new DefaultDungTriathlonSolver(createMssSolver(additionalParams), attacks, varMap, outputFormatter));
		default:
			return nullptr;
		}
	}

	static std::unique_ptr<SemanticsProblemSolver> getUndecoratedProblemInstance(Semantics semantic, TaskType task, std::map<std::string,std::string>& additionalParams, Attacks &attacks, VarMap &varMap, SolverOutputFormatter &outputFormatter) {
		if(task == TASK_UNDEFINED) return nullptr;
		switch(semantic.getName()) {
		case SEM_STABLE:
			return std::unique_ptr<SemanticsProblemSolver>(new DefaultStableSemanticsSolver(createSatSolver(additionalParams), attacks, varMap, task, outputFormatter));
		case SEM_COMPLETE:
			return std::unique_ptr<SemanticsProblemSolver>(new DefaultCompleteSemanticsSolver(createSatSolver(additionalParams), attacks, varMap, task, outputFormatter));
		case SEM_GROUNDED:
			return undecoratedGroundedSolver(task, additionalParams, attacks, varMap, outputFormatter);
		case SEM_PREFERRED:
			return std::unique_ptr<SemanticsProblemSolver>(new DefaultPreferredSemanticsSolver(createMssSolver(additionalParams), attacks, varMap, task, outputFormatter));
		case SEM_SEMISTABLE:
			return std::unique_ptr<SemanticsProblemSolver>(new DefaultSemistableSemanticsSolver(createMssSolver(additionalParams), attacks, varMap, task, outputFormatter));
		case SEM_STAGE:
			return std::unique_ptr<SemanticsProblemSolver>(new DefaultStageSemanticsSolver(createMssSolver(additionalParams), attacks, varMap, task, outputFormatter));
		case SEM_IDEAL:
			return std::unique_ptr<SemanticsProblemSolver>(new DefaultIdealSemanticsSolver(createMssSolver(additionalParams), attacks, varMap, task, outputFormatter));
		case SEM_TRIATHLON:
			return std::unique_ptr<SemanticsProblemSolver>(new DefaultDungTriathlonSolver(createMssSolver(additionalParams), attacks, varMap, outputFormatter));
		default:
			return nullptr;
		}
	}

};


}


#endif
