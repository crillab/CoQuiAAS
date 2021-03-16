#ifndef __SRC__ARG_SOLVERS__ICCMA19_SOLVER_OUTPUT_FORMATTER_H__
#define __SRC__ARG_SOLVERS__ICCMA19_SOLVER_OUTPUT_FORMATTER_H__

#include "SolverOutputFormatter.h"
#include "VarMap.h"
#include "Types.h"

namespace CoQuiAAS {

    class ICCMA19SolverOutputFormatter : public SolverOutputFormatter {

        public:

        ICCMA19SolverOutputFormatter(VarMap &varMap, void (*displayFct)(std::string));

        void writeArgAcceptance(bool status);

        void writeNoExt();

        void writeSingleExtension(std::vector<bool>& model);

        void writeSingleExtension(std::vector<int>& lits);

        void writeExtensionListBegin();

        void writeExtensionListElmt(std::vector<bool>& model, bool isFirst);

        void writeExtensionListElmt(std::vector<int>& lits, bool isFirst);

        void writeExtensionListEnd();

        void writeD3Begin();

        void writeD3GrExts(std::vector<int>& ext);

        void writeD3StExts(std::vector<std::vector<int> >& exts);

        void writeD3PrExts(std::vector<std::vector<int> >& exts);

        void writeD3End();

        void writeDynListBegin(TaskType task);

        void writeDynListElmtSep(TaskType task);

        void writeDynListEnd(TaskType task);

        virtual ~ICCMA19SolverOutputFormatter();

        protected:

        std::string formatSequenceOfExtensions(std::vector<std::string> exts);

    };
}

#endif