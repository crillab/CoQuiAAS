#ifndef __SRC__ARG_SOLVERS__ICCMA19_SOLVER_OUTPUT_FORMATTER_H__
#define __SRC__ARG_SOLVERS__ICCMA19_SOLVER_OUTPUT_FORMATTER_H__

#include "SolverOutputFormatter.h"
#include "VarMap.h"

namespace CoQuiAAS {

    class ICCMA19SolverOutputFormatter : public SolverOutputFormatter {

        public:

        ICCMA19SolverOutputFormatter(VarMap &varMap);

        std::string formatArgAcceptance(bool status);

        std::string formatNoExt();

        std::string formatSingleExtension(std::vector<bool>& model);

        std::string formatEveryExtension(std::vector<std::vector<bool>>& models);

        std::string formatSingleExtension(std::vector<int>& lits);

        std::string formatEveryExtension(std::vector<std::vector<int>>& lits);

        std::string formatD3(std::string grExts, std::string stExts, std::string prExts);

        virtual ~ICCMA19SolverOutputFormatter();

        protected:

        std::string formatSequenceOfExtensions(std::vector<std::string> exts);

        private:

        VarMap &vmap;

    };
}

#endif