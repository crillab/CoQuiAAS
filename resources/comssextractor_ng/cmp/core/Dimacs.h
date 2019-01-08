#ifndef CMP_Dimacs_h
#define CMP_Dimacs_h

#include <stdio.h>

#include "cmp/utils/ParseUtils.h"
#include "cmp/utils/cmpTypes.h"
#include "cmp/utils/wcnf.h"

using namespace CMP; 

//=================================================================================================
// DIMACS Parser:

template<class B> static bool readClause(B& in, WCNF& formula, vec<Lit>& lits, int hard = -1)
{  
  bool ret = false, first = hard != -1;    
  int parsed_lit, var;
  lits.clear();
  for ( ; ; )
    {
      parsed_lit = parseInt(in);
        
      if (parsed_lit == 0) break;
      if(hard != -1 && first){ret = (parsed_lit == hard); first = false;}
      else
	{
	  var = abs(parsed_lit) - 1;
	  while (var >= formula.nVars()) formula.newVar();
	  lits.push( (parsed_lit > 0) ? mkLit(var) : ~mkLit(var) );
	}
    }
  return ret;
}
  
template<class B> static void parse_DIMACS_main(B& in, WCNF &f)
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
	    }else if(*in == 'w' && eagerMatch(in, "wcnf"))
	    {
	      vars    = parseInt(in);
	      clauses = parseInt(in);
	      valueHard = parseInt(in);
	    }else printf("PARSE ERROR! Unexpected char: %c\n", *in), exit(3);
	} else if (*in == 'c' || *in == 'p') skipLine(in);
      else
	{
	  cnt++;
	  bool isHard = readClause(in, f, lits, valueHard);
	  if(isHard) {f.addHard(lits);}
	  else {f.addSoft(lits, cnt);}	
	}
    }
  if (vars != f.nVars()) fprintf(stderr, "c WARNING! DIMACS header mismatch: wrong number of variables.\n");
  if (cnt  != clauses) fprintf(stderr, "c WARNING! DIMACS header mismatch: wrong number of clauses.\n");
  //if (valueHard) fprintf(stderr, "c WARNING! partial maxsat problem.\n");
}// parse_DIMACS_main

// Inserts problem into solver.
static void parse_DIMACS(gzFile input_stream, WCNF &f) 
{
  StreamBuffer in(input_stream);
  parse_DIMACS_main(in, f); 
}
//=================================================================================================


#endif
