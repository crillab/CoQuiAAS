#include "CommandLineHelper.h"


#define CLH_MISS_FORMED_MSG "ERR:: WRONG USAGE\n\
CoQuiAAS invocation:\n\
  CoQuiAAS -p XX-YYY -fo ZZ -f instanceFile [-a variable] [OPTIONS]\n\
                       where XX in {SE, EE, DC, DS}\n\
                       where YY in {ST, CO, GR, PR, SST, STG}\n\
                       where ZZ in {apx, cnf, tgf}\n\
                       where \"-a variable\" must be present if XX in {DC, DS}\n\
\n\
  OPTIONS:\n\
    -externalSatSolver \"satSolver FILE\"     : launch an external SAT solver using the command \"satSolver FILE\" where FILE is replaced by a DIMACS cnf formatted file ; solver output must be compatible with SAT competitions output ; available for XX-ST and XX-CO problems\n\
    -externalCoMssSolver \"coMssSolver FILE\" : launch an external coMss solver using the command \"coMssSolver FILE\" where FILE is replaced by a DIMACS wcnf formatted file ; solver output must be compatible with coMSSExtractor output ; available for SE-PR and EE-PR problems\n\
    -externalMaxSatSolver \"maxSatSolver FILE\" : launch an external MaxSat solver using the command \"maxSatSolver FILE\" where FILE is replaced by a DIMACS wcnf formatted file ; solver output must be compatible with MaxSat competition ; available for SST and STG problems\n\
\n\
show authors and version:\n\
  CoQuiAAS\n\
\n\
show handled input formats:\n\
  CoQuiAAS --formats\n\
\n\
show handled problems:\n\
  CoQuiAAS --problems\n\
"
#ifndef COQUIAAS_VERSION
#define CLH_CREDITS_MSG "CoQuiAAS\n\
Compiled " __DATE__ " " __TIME__ "\n\
Jean-Marie Lagniez, Emmanuel Lonca, Jean-Guy Mailly {lagniez,lonca,mailly}@cril.fr"
#else
#define str(s) #s
#define xstr(s) str(s)
#define COQUIAAS_VERSION_STR xstr(COQUIAAS_VERSION)
#define CLH_CREDITS_MSG "CoQuiAAS v" COQUIAAS_VERSION_STR " \n\
Compiled " __DATE__ " " __TIME__ "\n\
Jean-Marie Lagniez, Emmanuel Lonca, Jean-Guy Mailly -- {lagniez,lonca}@cril.fr, jmailly@dbai.tuwien.ac.at"
#endif

#define CLH_SUPPORTED_FORMATS_MSG "[apx,cnf,tgf]"

#define CLH_SUPPORTED_PROBLEMS_MSG "[SE-ST,SE-CO,SE-GR,SE-PR,SE-SST,SE-STG,"\
									"EE-ST,EE-CO,EE-GR,EE-PR,EE-SST,EE-STG,"\
									"DC-ST,DC-CO,DC-GR,DC-PR,DC-SST,DC-STG,"\
									"DS-ST,DS-CO,DS-GR,DS-PR,DS-SST,DS-STG]"

#define FLAG_SET "FLAG_SET"


using namespace CoQuiAAS;


CommandLineHelper::CommandLineHelper(int argc, char** argv) {
  mustExit = errorOccured = false;
  semName = SEM_UNDEFINED;
  taskType = TASK_UNDEFINED;
  instanceFormat = FORMAT_UNDEFINED;
  instanceFile = "";
  for(int i=1; i<argc; ++i) {
    args.push_back(string(argv[i]));
  }
}

void CommandLineHelper::parseCommandLine() {
  if(args.size() == 0) {
    cout << CLH_CREDITS_MSG << endl ;
    mustExit = true;
    return;
  }
  for(unsigned int i=0; i<args.size(); ++i) {
    if(!args[i].compare("--problems")) {
      cout << CLH_SUPPORTED_PROBLEMS_MSG << endl;
      mustExit = true;
      return;
    }
    if(!args[i].compare("-p")) {
      if(!assertWellFormed(++i < args.size())) return;
      taskType = SolverFactory::getTaskType(args[i]);
      semName = SolverFactory::getSemanticName(args[i]);
      continue;
    }
    if(!args[i].compare("-fo")) {
      if(!assertWellFormed(++i < args.size())) return;
      instanceFormat = ParserFactory::getInstanceFormat(args[i]);
      continue;
    }
    if(!args[i].compare("--formats")) {
      cout << CLH_SUPPORTED_FORMATS_MSG << endl;
      mustExit = true;
      return;
    }
    if(!args[i].compare("-f")) {
      if(!assertWellFormed(++i < args.size())) return;
      instanceFile = args[i];
      continue;
    }
    if(args[i][0] == '-' && args[i].size()>1 && args[i][1] != '-') {
      if(!assertWellFormed(++i < args.size())) return;
      additionalParams.insert(pair<string,string>(args[i-1],args[i]));
      continue;
    }
    if(args[i][0] == '-' && args[i][1] == '-') {
      additionalParams.insert(pair<string,string>(args[i],FLAG_SET));
      continue;
    }
    assertWellFormed(false);
  }
  if (!errorOccured) assertWellFormed((taskType!=TASK_UNDEFINED) && (semName!=SEM_UNDEFINED) && (instanceFormat!=FORMAT_UNDEFINED) && (!instanceFile.empty()));
  if (!errorOccured) assertWellFormed((taskType!=TASK_CRED_INF) || (!additionalParams["-a"].empty()));
  if (!errorOccured) assertWellFormed((taskType!=TASK_SKEP_INF) || (!additionalParams["-a"].empty()));
}

SemanticName CommandLineHelper::getSemanticName() {
  return semName;
}

TaskType CommandLineHelper::getTaskType() {
  return taskType;
}

InstanceFormat CommandLineHelper::getInstanceFormat() {
  return instanceFormat;
}

string CommandLineHelper::getInstanceFile() {
  return instanceFile;
}

string CommandLineHelper::getAdditionalParameter(string param) {
  return additionalParams[param];
}

map<string,string> *CommandLineHelper::getAdditionalParams() {
  return &additionalParams;
}

bool CommandLineHelper::assertWellFormed(bool test) {
  if(test) return true;
  cerr << CLH_MISS_FORMED_MSG << endl;
  errorOccured = true;
  mustExit = true;
  return false;
}

bool CommandLineHelper::mustExitNow() {
  return mustExit;
}

bool CommandLineHelper::errorInCommandLine() {
  return errorOccured;
}

bool CommandLineHelper::outputProblemAndExit() {
  return additionalParams.find("--problem-output") != additionalParams.end();
}
