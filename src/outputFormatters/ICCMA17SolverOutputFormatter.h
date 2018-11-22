#ifndef __SRC__ARG_SOLVERS__ICCMA17_SOLVER_OUTPUT_FORMATTER_H__
#define __SRC__ARG_SOLVERS__ICCMA17_SOLVER_OUTPUT_FORMATTER_H__

#include "SolverOutputFormatter.h"
#include "VarMap.h"

namespace CoQuiAAS {

    class ICCMA17SolverOutputFormatter : public SolverOutputFormatter {

        public:

        ICCMA17SolverOutputFormatter(VarMap &varMap);

        std::string formatArgAcceptance(bool status);

        std::string formatNoExt();

        std::string formatSingleExtension(std::vector<bool>& model);

        std::string formatEveryExtension(std::vector<std::vector<bool>>& model);

        std::string formatSingleExtension(std::vector<int>& lits);

        std::string formatEveryExtension(std::vector<std::vector<int>>& lits);

        std::string formatD3(std::string grExts, std::string stExts, std::string prExts);

        virtual ~ICCMA17SolverOutputFormatter();

        private:

        VarMap &vmap;

    };
}

#endif