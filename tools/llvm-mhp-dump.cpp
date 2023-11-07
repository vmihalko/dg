#include "dg/llvm/PointerAnalysis/PointerAnalysis.h"
#include "dg/llvm/ThreadRegions/ControlFlowGraph.h"
#include "dg/llvm/ThreadRegions/MayHappenInParallel.h"
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_os_ostream.h>

#if LLVM_VERSION_MAJOR >= 4
#include <llvm/Bitcode/BitcodeReader.h>
#else
#include <llvm/Bitcode/ReaderWriter.h>
#endif

#include <fstream>
#include <functional>
#include <iostream>
#include <ostream>
#include <string>

enum class OutputFormat { dot, text, csv };

struct CommandLineOptions {
    std::string inputFile;
    std::string outputFile;
    OutputFormat outputFormat;

    std::ofstream outputFileStream;

    CommandLineOptions(std::string &inputFile, std::string &outputFile,
                       OutputFormat outputFormat)
            : inputFile(inputFile), outputFile(outputFile),
              outputFormat(outputFormat),
              outputFileStream(
                      (outputFile.empty() ? std::ofstream()
                       : (outputFormat == OutputFormat::csv)
                               ? std::ofstream(outputFile, std::ios_base::app)
                               : std::ofstream(outputFile))) {}

    std::ostream &getOstream() {
        if (outputFile.empty()) {
            return std::cout;
        }

        return outputFileStream;
    }
};

int main(int argc, char *argv[]) {
    using namespace llvm;

    cl::opt<std::string> outputFile("o", cl::desc("Specify output filename"),
                                    cl::value_desc("filename"), cl::init(""));

    cl::opt<std::string> inputFile(cl::Positional, cl::Required,
                                   cl::desc("<input file>"), cl::init(""));

    cl::opt<OutputFormat> outputFormat(
            "of",
            llvm::cl::desc("Specify which output format you want this tool to "
                           "generate"),
            llvm::cl::values(
                    clEnumValN(OutputFormat::dot, "dot",
                               "generates a graphviz graph"),
                    clEnumValN(OutputFormat::text, "text",
                               "print statistics about regions and MHP"),
                    clEnumValN(OutputFormat::csv, "csv",
                               "append MHP statistics to a CSV file")),
            cl::init(OutputFormat::dot));

    cl::ParseCommandLineOptions(argc, argv);

    auto opts = CommandLineOptions(inputFile, outputFile, outputFormat);

    std::string moduleFile = inputFile;
    std::string outputFileName = outputFile;

    llvm::LLVMContext context;
    llvm::SMDiagnostic SMD;

    std::unique_ptr<Module> M = llvm::parseIRFile(opts.inputFile, SMD, context);

    if (!M) {
        llvm::errs() << "Failed parsing '" << moduleFile << "' file:\n";
        SMD.print(argv[0], errs());
        return 1;
    }

    dg::DGLLVMPointerAnalysis pointsToAnalysis(M.get(), "main",
                                               dg::Offset::UNKNOWN, true);
    pointsToAnalysis.run();

    ControlFlowGraph controlFlowGraph(&pointsToAnalysis);
    controlFlowGraph.buildFunction(M->getFunction("main"));

    MayHappenInParallel mhp =
            MayHappenInParallel(controlFlowGraph.mainEntryRegion());
    mhp.run();

    std::ostream &stream = opts.getOstream();

    if (opts.outputFormat == OutputFormat::dot) {
        controlFlowGraph.printWithRegions(stream, &mhp);
        return 0;
    }

    // FIXME: this generates a set, which is not necessary; there should be a
    // method to only count the regions
    size_t regionCount = controlFlowGraph.allRegions().size();
    size_t maximumRelationCount = regionCount * regionCount;
    size_t actualRelationCount = mhp.countRelations();
    double percentageRemoved = 100.0 - 100.0 * (double) actualRelationCount /
                                               (double) maximumRelationCount;

    if (opts.outputFormat == OutputFormat::text) {
        stream << "Regions constructed: " << regionCount << '\n'
               << "Maximum possible number of MHP relations: "
               << maximumRelationCount << '\n'
               << "Number of MHP relations calculated: " << actualRelationCount
               << '\n'
               << "Percentage of MHP relations between regions removed: "
               << percentageRemoved << '\n';
    } else {
        stream << regionCount << ',' << maximumRelationCount << ','
               << actualRelationCount << ',' << percentageRemoved;
    }

    return 0;
}
