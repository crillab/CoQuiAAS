#ifndef __SRC__OUTPUT_FORMATTERS__SOLVER_OUTPUT_FORMATTER_FACTORY_H__
#define __SRC__OUTPUT_FORMATTERS__SOLVER_OUTPUT_FORMATTER_FACTORY_H__

#include <algorithm>

#include "SolverOutputFormatter.h"
#include "ICCMA17SolverOutputFormatter.h"
#include "ICCMA19SolverOutputFormatter.h"

namespace CoQuiAAS {

    class SolverOutputFormatterFactory {

        public:

        static std::unique_ptr<SolverOutputFormatter> getInstance(std::string str, VarMap &varMap, void (*displayFct)(std::string)) {
            std::transform(str.begin(), str.end(), str.begin(), ::toupper);
            if(!str.compare("ICCMA2017")) {
                return std::unique_ptr<SolverOutputFormatter>(new ICCMA17SolverOutputFormatter(varMap, displayFct));
            }
            if(!str.compare("ICCMA2019")) {
                return std::unique_ptr<SolverOutputFormatter>(new ICCMA19SolverOutputFormatter(varMap, displayFct));
            }
            return std::unique_ptr<SolverOutputFormatter>(new ICCMA19SolverOutputFormatter(varMap, displayFct));
        }

    };

}

#endif
