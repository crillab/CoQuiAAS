#ifndef metaSolver_h
#define metaSolver_h

namespace CMP
{
  class MetaSolver
  {
  public:
    MetaSolver(){}
    virtual void interrupt() = 0;
    virtual void printStats() = 0;
  };
}
#endif
