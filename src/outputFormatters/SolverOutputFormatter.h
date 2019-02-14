#ifndef __SRC__ARG_SOLVERS__SOLVER_OUTPUT_FORMATTER_H__
#define __SRC__ARG_SOLVERS__SOLVER_OUTPUT_FORMATTER_H__

#include <string>
#include <vector>

#include "VarMap.h"
#include "Types.h"

namespace CoQuiAAS {

    class SolverOutputFormatter {

        protected:
        
        SolverOutputFormatter(void (*displayFct)(std::string), VarMap& vmap) {
            this->displayFct = displayFct;
            this->vmap = &vmap;
        }

        std::string acceptance_status_str(bool status);

        std::string argArray(std::vector<bool>& model);

        std::string argArray(std::vector<int>& lits);

        const std::string YES_STR = "YES";

        const std::string NO_STR = "NO";

        void (*displayFct)(std::string);

        VarMap* vmap;

        public:

        inline void setVarMap(VarMap& vmap) {
            this->vmap = &vmap;
        }

        virtual void writeArgAcceptance(bool status) = 0;

        virtual void writeNoExt() = 0;

        virtual void writeSingleExtension(std::vector<bool>& model) = 0;

        virtual void writeSingleExtension(std::vector<int>& lits) = 0;

        virtual void writeExtensionListBegin() = 0;

        virtual void writeExtensionListElmt(std::vector<bool>& model, bool isFirst) = 0;

        virtual void writeExtensionListElmt(std::vector<int>& lits, bool isFirst) = 0;

        virtual void writeExtensionListEnd() = 0;

        virtual void writeD3Begin() = 0;

        virtual void writeD3GrExts(std::vector<int>& ext) = 0;

        virtual void writeD3StExts(std::vector<std::vector<int> >& exts) = 0;

        virtual void writeD3PrExts(std::vector<std::vector<int> >& exts) = 0;

        virtual void writeD3End() = 0;

        virtual void writeDynListBegin(TaskType task) = 0;

        virtual void writeDynListElmtSep(TaskType task) = 0;

        virtual void writeDynListEnd(TaskType task) = 0;

    };

}

#endif