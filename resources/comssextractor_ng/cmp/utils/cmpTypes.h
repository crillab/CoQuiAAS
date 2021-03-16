#ifndef CMP_SolverTypes_h
#define CMP_SolverTypes_h

#include <assert.h>

#include "cmp/utils/IntTypes.h"
#include "cmp/utils/Alg.h"
#include "cmp/utils/Vec.h"
//#include "cmp/utils/Alloc.h"

namespace CMP {

//=================================================================================================
// Variables, literals, lifted booleans, clauses:


// NOTE! Variables are just integers. No abstraction here. They should be chosen from 0..N,
// so that they can be used as array indices.

typedef int Var;
#define var_Undef (-1)


struct Lit {
    int     x;

    // Use this as a constructor:
    friend Lit mkLit(Var var, bool sign);
    friend Lit mkLit(Var var);

    bool operator == (Lit p) const { return x == p.x; }
    bool operator != (Lit p) const { return x != p.x; }
    bool operator <  (Lit p) const { return x < p.x;  } // '<' makes p, ~p adjacent in the ordering.
};


inline  Lit  mkLit     (Var var, bool sign = false) { Lit p; p.x = var + var + (int)sign; return p; }
inline  Lit  operator ~(Lit p)              { Lit q; q.x = p.x ^ 1; return q; }
inline  Lit  operator ^(Lit p, bool b)      { Lit q; q.x = p.x ^ (unsigned int)b; return q; }
inline  bool sign      (Lit p)              { return p.x & 1; }
inline  int  var       (Lit p)              { return p.x >> 1; }

inline void printLit   (Lit l)              {printf("%s%d ", sign(l) ? "-" : "", var(l)+1);}
inline void printLits  (const vec<Lit>& ps) {for(int i=0; i<ps.size(); i++) printLit(ps[i]); printf("\n");}

 
// Mapping Literals to and from compact integers suitable for array indexing:
inline  int  toInt     (Var v)              { return v; } 
inline  int  toInt     (Lit p)              { return p.x; } 
inline  Lit  toLit     (int i)              { Lit p; p.x = i; return p; } 

//const Lit lit_Undef = mkLit(var_Undef, false);  // }- Useful special constants.
//const Lit lit_Error = mkLit(var_Undef, true );  // }

const Lit lit_Undef = { -2 };  // }- Useful special constants.
const Lit lit_Error = { -1 };  // }


//=================================================================================================
// Lifted booleans:
//
// NOTE: this implementation is optimized for the case when comparisons between values are mostly
//       between one variable and one constant. Some care had to be taken to make sure that gcc 
//       does enough constant propagation to produce sensible code, and this appears to be somewhat
//       fragile unfortunately.

#define l_True  (lbool((uint8_t)0)) // gcc does not do constant propagation if these are real constants.
#define l_False (lbool((uint8_t)1))
#define l_Undef (lbool((uint8_t)2))

class lbool {
    uint8_t value;

public:
    explicit lbool(uint8_t v) : value(v) { }

    lbool()       : value(0) { }
    explicit lbool(bool x) : value(!x) { }

    bool  operator == (lbool b) const { return ((b.value&2) & (value&2)) | (!(b.value&2)&(value == b.value)); }
    bool  operator != (lbool b) const { return !(*this == b); }
    lbool operator ^  (bool  b) const { return lbool((uint8_t)(value^(uint8_t)b)); }

    lbool operator && (lbool b) const { 
        uint8_t sel = (this->value << 1) | (b.value << 3);
        uint8_t v   = (0xF7F755F4 >> sel) & 3;
        return lbool(v); }

    lbool operator || (lbool b) const {
        uint8_t sel = (this->value << 1) | (b.value << 3);
        uint8_t v   = (0xFCFCF400 >> sel) & 3;
        return lbool(v); }

    friend int   toInt  (lbool l);
    friend lbool toLbool(int   v);
};
inline int   toInt  (lbool l) { return l.value; }
inline lbool toLbool(int   v) { return lbool((uint8_t)v);  }

//=================================================================================================
// Clause -- a simple class for representing a clause:

 class Clause {
 private:
   struct{
     unsigned size  : 25;
     unsigned index : 32;
   } header;
  
   Lit* data;
  
 public:
   Clause(){}
   void init(const vec<Lit>& ps, int idx)
   {
     header.size = ps.size();
     header.index = idx;
     data = NULL;
     data = (Lit*) realloc(data, sizeof(Lit) * ps.size());
     for(int i=0; i<ps.size(); i++) data[i] = ps[i];     
   }
   
   Clause(const vec<Lit>& ps, int idx) {
     init(ps, idx);
   }

   // ~Clause() {assert(0);if(data != NULL) {free(data); data = NULL;}}
   ~Clause() {if(data != NULL) {free(data); data = NULL;}}

   int          index       ()      const   { return header.index; }
   int          size        ()      const   { return header.size;}  
   Lit&         operator [] (int i)         { return data[i]; }
   Lit          operator [] (int i) const   { return data[i]; }
 };

 inline void printClause(Clause c) {for(int i=0; i<c.size(); i++) {printLit(c[i]); printf(" ");} printf("\n");}

}//namespace

 
#endif
