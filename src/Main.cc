/**
 * \file Main.cc
 * \brief Main file for CoQuiAAS
 * \author Jean-Marie Lagniez, Emmanuel Lonca, Jean-Guy Mailly
 * \version 1.0
 * \date 14/10/2015
 *
 * Main file for CoQuiAAS: a solver for abstract argumentation problems
 * CoQuiAAS aims to resolve classical problems for abstract argumentation:
 * - Compute an extension of an AF given a semantics
 * - Compute all the extensions of an AF given a semantics
 * - Decide if an argument is skeptically accepted by an AF given a semantics
 * - Decide if an argument is credulously accepdted by an AF given a semantics
 *
 * Possible semantics are the usual ones from Dung's seminal paper: COMPLETE, PREFERRED, STABLE, GROUNDED
 *
 */


#include <memory>
#include <iostream>
#include <fstream>
#include <ctime>
#include <pthread.h>
#include <signal.h>

#include "SolverFactory.h"
#include "SemanticsProblemSolver.h"
#include "StatMapFactory.h"
#include "CommandLineHelper.h"
#include "IParser.h"
#include "ParserFactory.h"
#include "SolverOutputFormatter.h"
#include "SolverOutputFormatterFactory.h"


#define MAIN_UNSUPPORTED_FILE_FORMAT_MSG "ERR:: UNSUPPORTED FILE FORMAT"


using namespace CoQuiAAS;


pthread_t timeoutTh;
clock_t clk;


inline bool undefinedArgument(string arg, VarMap& map){
	return !map.contains(arg);
}


void setInitStats(CommandLineHelper& clh, std::unique_ptr<IParser> const &p);
void setFinalStats(CommandLineHelper& clh, std::unique_ptr<IParser> const &p);
void *handleTimeout(void *strSeconds);


int main(int argc, char** argv){

	clk = clock();

	CommandLineHelper clh = CommandLineHelper(argc, argv);
	clh.parseCommandLine();
	if(clh.mustExitNow()) return clh.errorInCommandLine() ? 1 : 0;
	StatMapFactory::createInstance(clh.getAdditionalParams().find("--printStats") == clh.getAdditionalParams().end());

	// handle timeout (if any)
	if(clh.getAdditionalParams().find("-timeout") != clh.getAdditionalParams().end()) {
		string strTimeout = clh.getAdditionalParameter("-timeout");
		pthread_create(&timeoutTh, NULL, handleTimeout, &strTimeout);
		pthread_detach(timeoutTh);
	}

	// parse instance depending on the format and the file
	ifstream file(clh.getInstanceFile().c_str(),ios::in);
	std::unique_ptr<IParser> parser = ParserFactory::getParserInstance(clh.getInstanceFormat(), &file);
	if(!parser) {
		cerr << MAIN_UNSUPPORTED_FILE_FORMAT_MSG << endl;
		return 2;
	}
	parser->parseInstance();
	file.close();

	// initialize the StatMap
	setInitStats(clh, parser);
	// request a semantic instance depending on the problem to compute
	SolverOutputFormatter& formatter = *SolverOutputFormatterFactory::getInstance(clh.getOutputFormatter(), parser->getVarMap());
	std::unique_ptr<SemanticsProblemSolver> problem = SolverFactory::getProblemInstance(clh.getSemanticName(), clh.getTaskType(), clh.getAdditionalParams(), parser->getAttacks(), parser->getVarMap(), formatter);
	if(!clh.getAdditionalParameter("-a").empty()){
		if(undefinedArgument(clh.getAdditionalParameter("-a"),parser->getVarMap())){
			cout << "UNDEFINED" << endl ;
			return -1;
		}
		problem->setAcceptanceQueryArgument(clh.getAdditionalParameter("-a"));
	}
	// init semantic with problem constraints
	problem->init();
	// compute the problem pointed at init time
	problem->compute();
	// display statistics (if StatMap is not "fake")
	// display solution depending of the problem pointed at init time
	std::cout << problem->answerToString() << std::endl;

	return 0;
}


void setInitStats(CommandLineHelper& clh, std::unique_ptr<IParser> const &parser) {
  std::shared_ptr<StatMap> statMap = make_shared<FakeStatMap>();
	statMap->setStat("solver", "CoQuiAAS v1.0");
	statMap->setStat("instance", clh.getInstanceFile());
	statMap->setStat("","");
	statMap->setStat("nArgs", (int)parser->getVarMap().nVars());
	statMap->setStat("nSelfAttacking", (int)parser->getVarMap().nSelfAttacking());
	statMap->setStat("nAttacks", (int)parser->getAttacks().nAttacks());
	statMap->setStat("maxAttacks", (int)parser->getAttacks().maxAttacks());
	statMap->setStat("","");
	statMap->setStat("graph density", (double)(parser->getAttacks().nAttacks())/(parser->getVarMap().nVars()*parser->getVarMap().nVars()));
	statMap->setStat("","");
}


void setFinalStats(CommandLineHelper& clh, std::unique_ptr<IParser> const &parser) {
	std::shared_ptr<StatMap> statMap = statMap;
	statMap->setStat("computation time (s)",(double)(clock()-clk)/CLOCKS_PER_SEC);
	statMap->printStats(stdout);
	fflush(stdout);
}


void sigIntHandler(int sig);
void *handleTimeout(void *strSeconds) {
	int nSec;
	struct sigaction sigact;
	int retVal = 0;
	std::shared_ptr<StatMap> statMap = statMap;

	sscanf(((string*)strSeconds)->c_str(), "%d", &nSec);
	if(nSec <= 0) {
		statMap->setStat("timeout (s)", "INVALID_VAL");
		retVal = 1;
		pthread_exit(&retVal);
		return NULL;
	}
	statMap->setStat("timeout (s)", nSec);
	memset(&sigact, 0, sizeof(struct sigaction));
	sigact.sa_handler = sigIntHandler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);
#ifdef SIGXCPU
	sigaction(SIGXCPU, &sigact, NULL);
#endif

	sleep(nSec);
	if(-1==kill(getpid(), SIGINT)) perror("kill (SIGINT)");
	pthread_exit(&retVal);
	return NULL;
}


void sigIntHandler(int sig) {
	std::shared_ptr<StatMap> statMap = statMap;
	statMap->setStat("timeout", "TRUE");
	statMap->setStat("computation time (s)",(double)(clock()-clk)/CLOCKS_PER_SEC);
	statMap->printStats(stdout);
	fflush(stdout);
	exit(10);
}
