#include "CommandLineHelper.h"
#include "Logger.h"


#define CLH_MISS_FORMED_MSG "ERR:: WRONG USAGE\n\
CoQuiAAS invocation:\n\
  CoQuiAAS -p XX-YY [-a variable] -fo ZZ -f instanceFile [OPTIONS]\n\
                  where XX-YYY in {D3} and -a is not present\n\
                  or\n\
                       where XX in {SE, EE, DC, DS}\n\
                       where YY in {ST, CO, GR, PR, SST, STG, ID}\n\
                       where \"-a variable\" must be present iff XX in {DC, DS}\n\
                  where ZZ in {apx, cnf, tgf}\n\
\n\
  OPTIONS:\n\
    -of XX : specify the output format where XX in {ICCMA17} (default: ICCMA17)\n\
    -externalSatSolver \"satSolver FILE\" : launch an external SAT solver using the command \"satSolver FILE\" where FILE is replaced by a DIMACS cnf formatted file ; solver output must be compatible with SAT competitions output ; available for XX-ST and XX-CO problems\n\
    -lbx \"path\" : launch an external lbx-like coMss solver located at \"path\" ; solver must handle \"-wm\" and \"-num n\" lbx options, and respect its input/output format ; mandatory for problems in the second level of the polynomial hierarchy\n\
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
Jean-Marie Lagniez, Emmanuel Lonca, Jean-Guy Mailly {lagniez,lonca}@cril.fr, jean-guy.mailly@parisdescartes.fr"
#else
#define str(s) #s
#define xstr(s) str(s)
#define COQUIAAS_VERSION_STR xstr(COQUIAAS_VERSION)
#define CLH_CREDITS_MSG "CoQuiAAS v" COQUIAAS_VERSION_STR " \n\
Compiled " __DATE__ " " __TIME__ "\n\
Jean-Marie Lagniez, Emmanuel Lonca, Jean-Guy Mailly -- {lagniez,lonca}@cril.fr, jean-guy.mailly@parisdescartes.fr"
#endif

#define CLH_SUPPORTED_FORMATS_MSG "[apx,cnf,tgf]"

#define CLH_SUPPORTED_PROBLEMS_MSG "[SE-ST,SE-CO,SE-GR,SE-PR,SE-SST,SE-STG,SE-ID,"\
									"EE-ST,EE-CO,EE-GR,EE-PR,EE-SST,EE-STG,EE-ID,"\
									"DC-ST,DC-CO,DC-GR,DC-PR,DC-SST,DC-STG,DC-ID,"\
									"DS-ST,DS-CO,DS-GR,DS-PR,DS-SST,DS-STG,DS-ID,D3]"

#define FLAG_SET "FLAG_SET"


using namespace CoQuiAAS;


CommandLineHelper::CommandLineHelper(int argc, char** argv) {
  mustExit = errorOccured = false;
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
      sem = SolverFactory::getSemantics(args[i]);
      continue;
    }
    if(!args[i].compare("-fo")) {
      if(!assertWellFormed(++i < args.size())) return;
      instanceFormat = ParserFactory::getInstanceFormat(args[i]);
      continue;
    }
    if(!args[i].compare("-of")) {
      if(!assertWellFormed(++i < args.size())) return;
      outputFormatter = args[i];
      continue;
    }
    if(!args[i].compare("-log")) {
      if(!assertWellFormed(++i < args.size())) return;
      Logger::getInstance()->addFile(args[i]);
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
    if(!args[i].compare("-m")) {
      if(!assertWellFormed(++i < args.size())) return;
      dynamicsFile = args[i];
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
  if (!errorOccured) assertWellFormed((taskType!=TASK_UNDEFINED) && (sem.getName()!=SEM_UNDEFINED) && (instanceFormat!=FORMAT_UNDEFINED) && (!instanceFile.empty()));
  if (!errorOccured) assertWellFormed((taskType!=TASK_CRED_INF) || (!additionalParams["-a"].empty()));
  if (!errorOccured) assertWellFormed((taskType!=TASK_SKEP_INF) || (!additionalParams["-a"].empty()));
}

Semantics CommandLineHelper::getSemantics() {
  return sem;
}

TaskType CommandLineHelper::getTaskType() {
  return taskType;
}

InstanceFormat CommandLineHelper::getInstanceFormat() {
  return instanceFormat;
}

string CommandLineHelper::getOutputFormatter() {
  return outputFormatter;
}

string CommandLineHelper::getInstanceFile() {
  return instanceFile;
}

string CommandLineHelper::getDynamicsFile() {
  return this->dynamicsFile;
}

string CommandLineHelper::getAdditionalParameter(string param) {
  return additionalParams[param];
}

map<string,string>& CommandLineHelper::getAdditionalParams() {
  return additionalParams;
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
