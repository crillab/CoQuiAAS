/****************************************************************************************[Dimacs.h]
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

#ifndef Minisat_Dimacs_h
#define Minisat_Dimacs_h

#include <stdio.h>

#include "utils/ParseUtils.h"
#include "core/SolverTypes.h"

namespace Minisat {

//=================================================================================================
// DIMACS Parser:

  template<class B, class Solver> static bool readClause(B& in, Solver& S, vec<Lit>& lits, int hard = -1) 
  {
    bool ret = true, first = hard != -1;    
    int parsed_lit, var;
    lits.clear();
    for ( ; ; )
      {
        parsed_lit = parseInt(in);
        if (parsed_lit == 0) break;
        if(hard != -1 && first){ret = (parsed_lit == hard); first = false;}
        else
          {
            var = abs(parsed_lit)-1;
            while (var >= S.nVars()) S.newVar();
            lits.push( (parsed_lit > 0) ? mkLit(var) : ~mkLit(var) );
          }        
      }
    return ret;
  }
  
  template<class B, class Solver> static void parse_DIMACS_main(B& in, Solver& S)
  {
    vec<Lit> lits;
    int vars = 0;
    int clauses = 0;
    int cnt = 0;
    int valueHard = -1;

    for (;;)
      {
        skipWhitespace(in);
        if (*in == EOF) break;
        else if(*in == 'p')
          {
            ++in; ++in; 
            if(*in == 'c' && eagerMatch(in, "cnf"))
              {
                vars    = parseInt(in);
                clauses = parseInt(in);
                S.setSoftInstance(false);
              }else if(*in == 'w' && eagerMatch(in, "wcnf"))
              {
                vars    = parseInt(in);
                clauses = parseInt(in);
                valueHard = parseInt(in);
                S.setSoftInstance(true);
              }else printf("PARSE ERROR! Unexpected char: %c\n", *in), exit(3);
          } else if (*in == 'c' || *in == 'p') skipLine(in);
        else
          {
            cnt++;
            bool isHard = readClause(in, S, lits, valueHard);
            S.addClause_(lits, isHard); 
          }
      }

    if (vars != S.nVars()) fprintf(stderr, "c WARNING! DIMACS header mismatch: wrong number of variables.\n");
    if (cnt  != clauses) fprintf(stderr, "c WARNING! DIMACS header mismatch: wrong number of clauses.\n");
    if (valueHard) fprintf(stderr, "c WARNING! partial maxsat problem.\n");
  }// parse_DIMACS_main

  // Inserts problem into solver.
  template<class Solver> static void parse_DIMACS(gzFile input_stream, Solver& S) 
  {
    StreamBuffer in(input_stream);
    parse_DIMACS_main(in, S); 
  }
 
//=================================================================================================
}

#endif
