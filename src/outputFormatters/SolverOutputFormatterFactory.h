#ifndef __SRC__OUTPUT_FORMATTERS__SOLVER_OUTPUT_FORMATTER_FACTORY_H__
#define __SRC__OUTPUT_FORMATTERS__SOLVER_OUTPUT_FORMATTER_FACTORY_H__

#include <algorithm>

#include "SolverOutputFormatter.h"
#include "ICCMA17SolverOutputFormatter.h"
#include "ICCMA19SolverOutputFormatter.h"

namespace CoQuiAAS {

    class SolverOutputFormatterFactory {

        public:

        static SolverOutputFormatter* getInstance(std::string str, VarMap &varMap) {
            std::transform(str.begin(), str.end(), str.begin(), ::toupper);
            if(!str.compare("ICCMA2017")) {
                return new ICCMA17SolverOutputFormatter(varMap);
            }
            if(!str.compare("ICCMA2019")) {
                return new ICCMA19SolverOutputFormatter(varMap);
            }
            return new ICCMA17SolverOutputFormatter(varMap);
        }

    };

}

#endif
