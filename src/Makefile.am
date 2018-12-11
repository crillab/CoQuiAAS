AM_CPPFLAGS = -g -Wall -Wno-unknown-warning-option -Wno-parentheses -D __STDC_LIMIT_MACROS -D __STDC_FORMAT_MACROS -I$(srcdir)/outputFormatters -I$(srcdir)/specs -I$(srcdir)/utils -I$(srcdir)/parser -I$(srcdir)/satSolvers -I$(srcdir)/argSolvers -I$(top_srcdir)/resources/comssextractor/

## CoQuiAAS source files
satSolversDirSources = satSolvers/SatSolver.h satSolvers/MssSolver.h satSolvers/ExternalSatSolver.cc satSolvers/ExternalSatSolver.h satSolvers/BuiltInMssSolver.cc satSolvers/BuiltInMssSolver.h satSolvers/BuiltInSatSolver.cc satSolvers/BuiltInSatSolver.h satSolvers/MaxSatSolver.h satSolvers/ExternalMaxSatSolver.h satSolvers/ExternalMaxSatSolver.cc satSolvers/LbxCoMssSolver.h satSolvers/LbxCoMssSolver.cc satSolvers/ExternalCoMssSolver.h satSolvers/ExternalCoMssSolver.cc
argSolversDirSources = argSolvers/SolverFactory.h argSolvers/SatEncodingHelper.cc argSolvers/SatEncodingHelper.h argSolvers/MssEncodingHelper.cc argSolvers/MssEncodingHelper.h argSolvers/MaxSatEncodingHelper.cc argSolvers/MaxSatEncodingHelper.h argSolvers/DefaultCompleteSemanticsSolver.cc argSolvers/DefaultCompleteSemanticsSolver.h argSolvers/SemanticsProblemSolver.cc argSolvers/SemanticsProblemSolver.h argSolvers/DefaultGroundedSemanticsSolver.cc argSolvers/DefaultGroundedSemanticsSolver.h argSolvers/GraphBasedGroundedSemanticsSolver.cc argSolvers/GraphBasedGroundedSemanticsSolver.h argSolvers/DefaultPreferredSemanticsSolver.cc argSolvers/DefaultPreferredSemanticsSolver.h argSolvers/DefaultStableSemanticsSolver.cc argSolvers/DefaultStableSemanticsSolver.h argSolvers/DefaultSemistableSemanticsSolver.cc argSolvers/DefaultSemistableSemanticsSolver.h argSolvers/DefaultStageSemanticsSolver.cc argSolvers/DefaultStageSemanticsSolver.h argSolvers/DefaultIdealSemanticsSolver.cc argSolvers/DefaultIdealSemanticsSolver.h argSolvers/DefaultDungTriathlonSolver.cc argSolvers/DefaultDungTriathlonSolver.h argSolvers/DynamicSemanticsSolverDecorator.cc argSolvers/DynamicSemanticsSolverDecorator.h
outputFormattersDirSources = ./outputFormatters/SolverOutputFormatter.h ./outputFormatters/SolverOutputFormatter.cc ./outputFormatters/SolverOutputFormatterFactory.h ./outputFormatters/ICCMA17SolverOutputFormatter.h ./outputFormatters/ICCMA17SolverOutputFormatter.cc ./outputFormatters/ICCMA19SolverOutputFormatter.h ./outputFormatters/ICCMA19SolverOutputFormatter.cc
specsDirSources = ./specs/VarMap.cc ./specs/VarMap.h ./specs/Attacks.cc ./specs/Attacks.h ./specs/Types.h ./specs/Semantics.h
parserDirSources = ./parser/AspartixFormatParser.cc ./parser/AspartixFormatParser.h ./parser/CNFFormatParser.cc ./parser/CNFFormatParser.h ./parser/TrivialGraphFormatParser.cc ./parser/TrivialGraphFormatParser.h ./parser/IParser.h ./parser/ParserFactory.h
utilsDirSources= ./utils/DefaultStatMap.cc ./utils/DefaultStatMap.h ./utils/FakeStatMap.cc ./utils/FakeStatMap.h ./utils/CommandLineHelper.cc ./utils/CommandLineHelper.h ./utils/LogStatMap.cc ./utils/LogStatMap.h ./utils/StatMap.h ./utils/StatMapFactory.h ./utils/ExtensionUtils.cc ./utils/ExtensionUtils.h
srcDirSources = ./Main.cc

## comssextractor object files
comssextractorObjs = $(top_builddir)/resources/comssextractor/utils/Options.o $(top_builddir)/resources/comssextractor/utils/System.o $(top_builddir)/resources/comssextractor/core/SolverRL.o $(top_builddir)/resources/comssextractor/core/Solver.o $(top_builddir)/resources/comssextractor/core/Main.o


noinst_LIBRARIES = $(top_builddir)/lib/libcoquiaas.a
__top_builddir__lib_libcoquiaas_a_SOURCES = $(specsDirSources) $(parserDirSources) $(argSolversDirSources) $(satSolversDirSources) $(utilsDirSources) $(outputFormattersDirSources)
__top_builddir__lib_libcoquiaas_a_LIBADD = $(comssextractorObjs)


bin_PROGRAMS = $(top_builddir)/bin/CoQuiAAS
__top_builddir__bin_CoQuiAAS_SOURCES = $(srcDirSources)
__top_builddir__bin_CoQuiAAS_LDADD = $(top_builddir)/lib/libcoquiaas.a
__top_builddir__bin_CoQuiAAS_LDFLAGS = -lpthread
