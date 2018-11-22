#ifndef __SRC__ARG_SOLVERS__SOLVER_OUTPUT_FORMATTER_H__
#define __SRC__ARG_SOLVERS__SOLVER_OUTPUT_FORMATTER_H__

#include <string>
#include <vector>

namespace CoQuiAAS {

    class SolverOutputFormatter {

        protected:
        
        SolverOutputFormatter() {}

        public:

        virtual std::string formatArgAcceptance(bool status) = 0;

        virtual std::string formatNoExt() = 0;

        virtual std::string formatSingleExtension(std::vector<bool>& model) = 0;

        virtual std::string formatEveryExtension(std::vector<std::vector<bool>>& model) = 0;

        virtual std::string formatSingleExtension(std::vector<int>& lits) = 0;

        virtual std::string formatEveryExtension(std::vector<std::vector<int>>& lits) = 0;

        virtual std::string formatD3(std::string grExts, std::string stExts, std::string prExts) = 0;

    };

}

#endif