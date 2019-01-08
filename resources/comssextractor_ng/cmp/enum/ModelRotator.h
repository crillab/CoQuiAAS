#ifndef ModelRotator_h
#define ModelRotator_h

#include <map>
#include <vector>

#include "cmp/utils/wcnf.h"
#include "cmp/utils/cmpTypes.h"
#include "cmp/ifaces/SatSolver.h"

using namespace std;

namespace CMP {

  class RotateModel; 

  class ModelRotator {

    WCNF& formula;
    vector<int*>& orgSelector_map;
    vector<int>& freshSelector_map;
    vector<int*>& orgID2sel_map;
    vector< vector<int> > s_occurs;
    vector< vector<int> > h_occurs;
    vector<int> visited;
    vector<bool> visited_var;
    vector<int> cand;
    vector<int> actived;
    int uns_sft;
    int uns_hrd;
    bool singletonSft;
    bool singletonHrd;
    int stamp_cand;
    int stamp_core;
    
  public:
    ModelRotator(WCNF& orgFormula, vector<int*>& orgSelector_map, vector<int>& freshSelector_map, vector<int*>& orgID2sel_map);
    // virtual ~ModelRotator();
    
    void process(RotateModel& rm, bool reset=true);
    void updateSatCore(vec<Lit>& core);
    //void add_clB(const vec<Lit>& lits);
    
  protected:
    
    void setup(RotateModel& _rm);
    void flip(RotateModel& rm, Var x);
    bool satisfy_clS(RotateModel& rm, Var x);
    bool satisfy_clH(RotateModel& rm, Var x);
    //bool satisfy_clB(RotateModel& rm, Var x);
    bool satisfied(RotateModel& rm, const vector<Lit>& cl);
    int ex2in(Lit s);
    Lit clID2sel(int i);

    inline bool freshVar(Var v) {return (v>=formula.nVars());}
  };

  class RotateModel {
    friend class ModelRotator;
  private:
    vector<lbool>& model;
    const vec<Lit>& core;
    const Lit tc;
    vector<int> crits;

    inline lbool modelValue(Var x) {return model[x];}
    inline lbool modelValue(Lit p) {return (model[(var(p))] ^ sign(p));}

  public:
  RotateModel(vector<lbool>& m, const vec<Lit>& c, const Lit t) : model(m), core(c), tc(t) {}
    
    //virtual ~RotateModel() {}
    inline vector<int>& get_crits() {return crits;}
  };
  
}//namespace

#endif
