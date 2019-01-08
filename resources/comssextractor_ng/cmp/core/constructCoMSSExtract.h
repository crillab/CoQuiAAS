#ifndef constructCoMSSExtract_h
#define constructCoMSSExtract_h

#include "cmp/core/coMSSExtract.h"

using namespace std;

namespace CMP {
class ConstructCoMSSExtract : public CoMSSExtract
{
 protected:
  
 public:
  ConstructCoMSSExtract(WCNF &f);
  void search(vec<Lit>& mss, vec<Lit>& unknown, vec<Lit> &coMss);    
};
 
}//namespace

#endif
