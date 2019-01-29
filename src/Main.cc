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
#include <tuple>

#include "SolverFactory.h"
#include "SemanticsProblemSolver.h"
#include "StatMapFactory.h"
#include "CommandLineHelper.h"
#include "IParser.h"
#include "ParserFactory.h"
#include "SolverOutputFormatter.h"
#include "SolverOutputFormatterFactory.h"
#include "Logger.h"


using namespace CoQuiAAS;


pthread_t timeoutTh;
clock_t clk;


inline bool undefinedArgument(std::string arg, VarMap& map){
	return !map.contains(arg);
}


void setInitStats(CommandLineHelper& clh, std::unique_ptr<IParser> const &p);
void setFinalStats(CommandLineHelper& clh, std::unique_ptr<IParser> const &p);
void *handleTimeout(void *strSeconds);

void manageTimeout(CommandLineHelper& clh);
InstanceFormat manageInstanceFormat(CommandLineHelper& clh);
std::ifstream manageInstanceFile(CommandLineHelper& clh);


int main(int argc, char** argv){

	clk = clock();

	CommandLineHelper clh = CommandLineHelper(argc, argv);
	clh.parseCommandLine();
	Logger::getInstance()->info("CoQuiAAS start");
	if(clh.mustExitNow()) return clh.errorInCommandLine() ? 1 : 0;
	StatMapFactory::createInstance(clh.getAdditionalParams().find("--printStats") == clh.getAdditionalParams().end());

	manageTimeout(clh);
	InstanceFormat instanceFormat = manageInstanceFormat(clh);
	std::ifstream file = manageInstanceFile(clh);

	std::unique_ptr<IParser> parser = ParserFactory::getParserInstance(instanceFormat, &file);
	parser->parseInstance();
	if(clh.getSemantics().isDynamic()) {
		std::ifstream dynfile(clh.getDynamicsFile().c_str(),std::ios::in);
		parser->parseDynamics(&dynfile);
		dynfile.close();
	}
	file.close();

	// initialize the StatMap
	setInitStats(clh, parser);
	// request a semantic instance depending on the problem to compute
	std::unique_ptr<SolverOutputFormatter> formatter = SolverOutputFormatterFactory::getInstance(clh.getOutputFormatter(), parser->getVarMap(), [] (std::string s) {std::cout << s; std::cout.flush();});
	if(clh.getSemantics().getName() == SEM_UNDEFINED || clh.getTaskType() == TASK_UNDEFINED) {
		Logger::getInstance()->fatal("unsupported task: %s", clh.getTask().c_str());
		std::cerr << CommandLineHelper::USAGE << std::endl;
		return 1;
	}
	std::unique_ptr<SemanticsProblemSolver> problem = SolverFactory::getProblemInstance(clh.getSemantics(), clh.getTaskType(), clh.getAdditionalParams(), parser->getAttacks(), parser->getVarMap(), *formatter);
	if(!clh.getAdditionalParameter("-a").empty()){
		if(undefinedArgument(clh.getAdditionalParameter("-a"),parser->getVarMap())){
			std::cout << "UNDEFINED" << std::endl ;
			return -1;
		}
		problem->setAcceptanceQueryArgument(clh.getAdditionalParameter("-a"));
	}
	// init semantic with problem constraints
	problem->init();
	// compute the problem pointed at init time
	problem->compute();
	// display statistics (if StatMap is not "fake")

	std::cout << std::endl;
	Logger::getInstance()->info("global computation time: %.3fs", (double)(clock()-clk)/CLOCKS_PER_SEC);
	Logger::getInstance()->info("CoQuiAAS end");
	return 0;
}


void manageTimeout(CommandLineHelper& clh) {
	if(clh.getAdditionalParams().find("-timeout") != clh.getAdditionalParams().end()) {
		std::string strTimeout = clh.getAdditionalParameter("-timeout");
		Logger::getInstance()->info("timeout set to %s seconds", strTimeout);
		pthread_create(&timeoutTh, NULL, handleTimeout, &strTimeout);
		pthread_detach(timeoutTh);
	} else {
		Logger::getInstance()->info("no timeout set");
	}
}


InstanceFormat manageInstanceFormat(CommandLineHelper& clh) {
	InstanceFormat instanceFormat = ParserFactory::getInstanceFormat(clh.getInstanceFormat());
	if(instanceFormat == FORMAT_UNDEFINED) {
		Logger::getInstance()->fatal("unsupported file format: %s", clh.getInstanceFormat().c_str());
		std::cerr << CommandLineHelper::USAGE << std::endl;
		exit(1);
	}
	return instanceFormat;
}


std::ifstream manageInstanceFile(CommandLineHelper& clh) {
	std::ifstream file(clh.getInstanceFile().c_str(),std::ios::in);
	if(!file.good()) {
		Logger::getInstance()->fatal("cannot read input file: %s", clh.getInstanceFile().c_str());
		std::cerr << CommandLineHelper::USAGE << std::endl;
		exit(1);
	}
	char *absPath = realpath(clh.getInstanceFile().c_str(), NULL);
	Logger::getInstance()->info("input file is %s", absPath);
	free(absPath);
	return file;
}


void setInitStats(CommandLineHelper& clh, std::unique_ptr<IParser> const &parser) {
  std::shared_ptr<StatMap> statMap = std::make_shared<FakeStatMap>();
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

	sscanf(((std::string*)strSeconds)->c_str(), "%d", &nSec);
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
