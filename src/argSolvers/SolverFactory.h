#ifndef __SOLVER_FACTORY_H__
#define __SOLVER_FACTORY_H__


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
#include "BuiltInSatSolver.h"
#include "ExternalSatSolver.h"
#include "BuiltInMssSolver.h"
#include "LbxCoMssSolver.h"
#include "ExternalMaxSatSolver.h"


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
		str = str.substr(0,2);
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
		if(!str.compare("DT")) return TASK_TRIATHLON;
		if(!str.compare("SE")) return TASK_ONE_EXT;
		if(!str.compare("EE")) return TASK_ALL_EXTS;
		if(!str.compare("DC")) return TASK_CRED_INF;
		if(!str.compare("DS")) return TASK_SKEP_INF;
		return TASK_UNDEFINED;
	}

	/**
	 * \fn getSemanticName(str)
	 * \brief try to recognize a semantic name in a string
	 * \param str : the string to parse
	 */
	static SemanticName getSemanticName(std::string str) {
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
		if(!str.compare("DT")) return SEM_TRIATHLON;
		str = str.substr(3);
		if(!str.compare("ST")) return SEM_STABLE;
		if(!str.compare("CO")) return SEM_COMPLETE;
		if(!str.compare("GR")) return SEM_GROUNDED;
		if(!str.compare("PR")) return SEM_PREFERRED;
		if(!str.compare("SST")) return SEM_SEMISTABLE;
		if(!str.compare("STG")) return SEM_STAGE;
		if(!str.compare("ID")) return SEM_IDEAL;
		return SEM_UNDEFINED;
	}

	static SatSolver *createSatSolver(std::map<std::string,std::string> *additionalParams) {
		if(additionalParams->find("-externalSatSolver") != additionalParams->end()) {
			return new ExternalSatSolver((*additionalParams)["-externalSatSolver"]);
		}
		return new BuiltInSatSolver();
	}

	static MssSolver *createMssSolver(std::map<std::string,std::string> *additionalParams) {
		if(additionalParams->find("-lbx") != additionalParams->end()) {
			return new LbxCoMssSolver((*additionalParams)["-lbx"]);
		}
		std::cerr << "ERROR:: no builtin coMSS solver" << std::endl;
		std::exit(1);
	}

	static MaxSatSolver *createMaxSatSolver(std::map<std::string,std::string> *additionalParams) {
		if(additionalParams->find("-externalMaxSatSolver") != additionalParams->end()) {
			return new ExternalMaxSatSolver((*additionalParams)["-externalMaxSatSolver"]);
		}
		std::cerr << "ERROR:: no builtin MaxSAT solver" << std::endl;
		std::exit(1);
	}

	static SemanticsProblemSolver *groundedSolver(TaskType task, std::map<std::string,std::string> *additionalParams, Attacks &attacks, VarMap &varMap) {
		if(additionalParams->find("--graphBased") != additionalParams->end()) {
			return new GraphBasedGroundedSemanticsSolver(attacks, varMap, task);
		}
		return new DefaultGroundedSemanticsSolver(*createSatSolver(additionalParams), attacks, varMap, task);
	}

	/**
	 * \fn getProblemInstance(SemanticName,TaskType,std::string)
	 * \brief return a new problem instance given a semantic and a task ; also provide the additional parameter if necessary (case of DC or DS)
	 * \param semantic : the semantic of the argumentation framework
	 * \param task : the task that is required
	 * \param additionalParams the additional parameters from the command line
	 */
	static SemanticsProblemSolver *getProblemInstance(SemanticName semantic, TaskType task, std::map<std::string,std::string> *additionalParams, Attacks &attacks, VarMap &varMap) {
		if(task == TASK_UNDEFINED) return NULL;
		switch(semantic) {
		case SEM_STABLE:
			return new DefaultStableSemanticsSolver(*createSatSolver(additionalParams), attacks, varMap, task);
		case SEM_COMPLETE:
			return new DefaultCompleteSemanticsSolver(*createSatSolver(additionalParams), attacks, varMap, task);
		case SEM_GROUNDED:
			return groundedSolver(task, additionalParams, attacks, varMap);
		case SEM_PREFERRED:
			return new DefaultPreferredSemanticsSolver(*createMssSolver(additionalParams), attacks, varMap, task);
		case SEM_SEMISTABLE:
			return new DefaultSemistableSemanticsSolver(*createMssSolver(additionalParams), attacks, varMap, task);
		case SEM_STAGE:
			return new DefaultStageSemanticsSolver(*createMssSolver(additionalParams), attacks, varMap, task);
		case SEM_IDEAL:
			return new DefaultIdealSemanticsSolver(*createMssSolver(additionalParams), attacks, varMap, task);
		case SEM_TRIATHLON:
			return new DefaultDungTriathlonSolver(*createMssSolver(additionalParams), attacks, varMap);
		default:
			return NULL;
		}
	}

};


}


#endif
