/*****************************************************************************************[Main.cc]
Copyright (c) 2003-2006, Niklas Een, Niklas Sorensson
Copyright (c) 2007-2010, Niklas Sorensson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************************/

#include <errno.h>

#include <signal.h>
#include <zlib.h>

#include "cmp/utils/System.h"
#include "cmp/utils/ParseUtils.h"
#include "cmp/utils/Options.h"

#include "cmp/core/Dimacs.h"
#include "cmp/core/constructCoMSSExtract.h"
#include "cmp/core/destructCoMSSExtract.h"
#include "cmp/enum/coMSSEnum.h"

#include "cmp/enum/constructCoMSSExtract4Enum.h"
#include "cmp/enum/destructCoMSSExtract4Enum.h"


using namespace CMP;
//=================================================================================================


static MetaSolver* msolver;

// Terminate by notifying the solver and back out gracefully. This is mainly to have a test-case
// for this feature of the Solver as it may take longer than an immediate call to '_exit()'.
static void SIGINT_interrupt(int signum) {printf("\n*** INTERRUPTED ***\n"); msolver->interrupt(); }

// Note that '_exit()' rather than 'exit()' has to be used. The reason is that 'exit()' calls
// destructors and may cause deadlocks if a malloc/free function happens to be running (these
// functions are guarded by locks for multithreaded use).
static void SIGINT_exit(int signum)
{
 printf("c\n"); printf("*** INTERRUPTED ***\n");  
 if(msolver) {msolver->printStats(); printf("*** INTERRUPTED ***\n"); fflush(stdout);}
  _exit(1);
}// SIGINT_exit

static void SIGTERM_exit(int signum) {
  printf("\n*** TERMINED ***\n");
  if (msolver != NULL) {msolver->printStats(); printf("*** TERMINED ***\n"); fflush(stdout);}
  _exit(1);
}//SIGTERM_exit


//=================================================================================================
// Main:
int main(int argc, char** argv)
{
  try 
    {
      setUsageHelp("USAGE: %s [options] <input-file> \n\n  where input may be either in plain or gzipped DIMACS.\n");
    
#if defined(__linux__) && defined __GLIBC__
      fpu_control_t oldcw, newcw;
      _FPU_GETCW(oldcw); newcw = (oldcw & ~_FPU_EXTENDED) | _FPU_DOUBLE; _FPU_SETCW(newcw);
      //printf("c WARNING: for repeatability, setting FPU to use double precision\n");
#endif
      // Extra options:
      //
      IntOption verb   ("MAIN", "verb", "Verbosity level (0=silent, 1=some, 2=more).", 1, IntRange(0, 2));
      IntOption cpu_lim("MAIN", "cpu-lim", "Limit on CPU time allowed in seconds.\n", INT32_MAX, IntRange(0, INT32_MAX));
      IntOption mem_lim("MAIN","mem-lim", "Limit on memory usage in megabytes.\n",INT32_MAX,IntRange(0,INT32_MAX));
      IntOption coMssAlgo ("MAIN", "algo", "select the algorithm: 1: constructive, 2: destructive\n", 1, IntRange(1, 2));
      IntOption coMssApprox ("MAIN", "approx", "select the approx: 0: no-approx, 1: basic, 2: enhanced\n", 0, IntRange(0, 2));
      IntOption nb_enum("MAIN","nb", "Maximal number of coMSS to extract.\n",INT32_MAX,IntRange(0,INT32_MAX));
      
      //BoolOption extractCoMss("MAIN", "extractCoMss", "Extract a coMSS of the formula", true);
      BoolOption optClauseD("MAIN", "clD", "Use disjuctive clause technique to extend the MSS", true);
      BoolOption optBackbone("MAIN", "bb", "Add backbone literals in the formula", true);
      BoolOption optClauseN("MAIN", "clN", "Add the negation clause in the assumptions", true);
      BoolOption coMssEnum ("MAIN", "enum", "Enumeration of all coMSS", false);
      BoolOption model_rotation ("MAIN", "mr", "Use model rotation to enumerate coMSS", false);
      BoolOption caching ("MAIN", "cache", "Use caching to enumerate coMSS", false);

      
      
// #if 0
      IntOption local_search ("MAIN", "ls", "local search: 0: no-ls, 1: oneDescent, 2: wsat\n", 0, IntRange(0, 2));
      IntOption conf_lim("MAIN","conf-lim", "Limit on conflict in approx.\n",INT32_MAX,IntRange(0,INT32_MAX));
      
// #endif
      
      parseOptions(argc, argv, true);
    
      double initial_time = cpuTime();       

      // Use signal handlers that forcibly quit until the solver will be able to respond to
      // interrupts:
      msolver = NULL;
      signal(SIGINT, SIGINT_exit);
      signal(SIGTERM, SIGTERM_exit);
      //signal(SIGXCPU,SIGINT_exit);

      // Set limit on CPU-time:
      if (cpu_lim != INT32_MAX)
        {
          rlimit rl;
          getrlimit(RLIMIT_CPU, &rl);
          if (rl.rlim_max == RLIM_INFINITY || (rlim_t)cpu_lim < rl.rlim_max)
            {
              rl.rlim_cur = cpu_lim;
              if (setrlimit(RLIMIT_CPU, &rl) == -1) printf("WARNING! Could not set resource limit: CPU-time.\n");
            } 
        }
    
      // Set limit on virtual memory:
      if (mem_lim != INT32_MAX)
        {
          rlim_t new_mem_lim = (rlim_t)mem_lim * 1024*1024;
          rlimit rl;
          getrlimit(RLIMIT_AS, &rl);
          if (rl.rlim_max == RLIM_INFINITY || new_mem_lim < rl.rlim_max)
            {
              rl.rlim_cur = new_mem_lim;
              if (setrlimit(RLIMIT_AS, &rl) == -1) printf("WARNING! Could not set resource limit: Virtual memory.\n");
            } 
        }


      if(argc < 2) {printf("Use '--help' for help.\n"); exit(0);}      
      if (argc == 1) printf("Reading from standard input... Use '--help' for help.\n");
    
      gzFile in = (argc == 1) ? gzdopen(0, "rb") : gzopen(argv[1], "rb");
      if (in == NULL) printf("ERROR! Could not open file: %s\n", argc == 1 ? "<stdin>" : argv[1]), exit(1);

      WCNF formula = WCNF();
      parse_DIMACS(in, formula);
      gzclose(in);


      printf("c Number of variables: %d\n", formula.nVars());
      printf("c Number of soft clauses: %d\n", formula.nSofts());
      printf("c Number of hard clauses: %d\n", formula.nHards());
      
      double parsed_time = cpuTime();
      printf("c Parse time (sec): %.2f\n", parsed_time - initial_time);
      printf("c\n");
      
      // Change to signal-handlers that will only notify the solver and allow it to terminate
      // voluntarily:
      signal(SIGINT, SIGINT_interrupt);
      signal(SIGTERM, SIGTERM_exit);
      //signal(SIGXCPU,SIGINT_interrupt);

      if(coMssEnum)
	{
	  CoMSSEnum* mcsEnum = NULL;
	  Config_CoMSSEnum config =  Config_CoMSSEnum();
	  config.alg= (coMssAlgo == 1)? CONSTRUCTIVE : DESTRUCTIVE;
	  config.clD= (optClauseD) ? true : false;
	  config.clN= (optClauseN) ? true : false;
	  config.bb= (optBackbone) ? true : false;
	  config.mr= (model_rotation);
	  config.cache = (caching);
	  //config.appx = (coMssApprox);
	  config.appx = 0;
	  config.verb = verb;
	  config.nb = nb_enum;
	  mcsEnum = new CoMSSEnum(formula, config);
	  msolver = mcsEnum;
	  mcsEnum->run();
	  mcsEnum->printStats();
	}
      else
        {
          CoMSSExtract *mcsExttor = NULL;
          if(coMssAlgo == 1) mcsExttor = new ConstructCoMSSExtract(formula);
          else if(coMssAlgo == 2) mcsExttor = new DestructCoMSSExtract(formula);
	  else exit(0);
	  msolver = mcsExttor;
          assert(mcsExttor);
	  mcsExttor->configOpt(optBackbone, optClauseD, optClauseN, coMssApprox, verb, conf_lim, local_search);

	  if(optBackbone) printf("c bb\n");
	  if(optClauseD) printf("c clD\n");
	  if(optClauseN) printf("c clN\n");

          vec<Lit> mcs;
	  (*mcsExttor)(mcs);
	  mcsExttor->printModel(mcs);
        }
      
    } catch (OutOfMemoryException&)
    {
      printf("===============================================================================\n");
      printf("memory out\n");
      exit(0);
    }
}
