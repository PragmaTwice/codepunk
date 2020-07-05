#include <string>
#include <llvm/Support/CommandLine.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>

#include "IntervalAnalysis.h"

using namespace llvm;

static cl::opt<std::string> InputFilename(cl::Positional, cl::desc("filename of LLVM IR input"));
static cl::opt<int> MaxIteration("iterate", cl::desc("max iteration count"),
        cl::value_desc("number"), cl::init(-1));

raw_ostream &operator<<(raw_ostream& o, const Value *v) {
    if(v->hasName()) {
        return o << v->getName() << " <" << (void *)v << ">";
    }
    if(auto c = dyn_cast<Constant>(v)) {
        return o << c->getUniqueInteger() << " <" << (void *)v << ">";
    }

    return o << (void *)v;
}

void analyze(const Function* f, int maxIteration) {
    IntervalAnalysis analysis(f);
    analysis.analyze(maxIteration);

    outs() << f->getName() << ":\n";
    for(const auto& v : f->args()) {
        outs() << "  | " << &v;
    }
    outs() << "\n";
    for(const auto& bb : f->getBasicBlockList()) {
        outs() << "  [" << &bb << "]\n";

        for(const auto& inst : bb.getInstList()) {
            outs() << "\t" << inst.getOpcodeName() << " (";
            bool isF = true;
            for(const auto& v : inst.operands()) {
                if(isF) isF = false;
                else outs() << ", ";

                outs() << v;
            }
            outs() << ")->" << &inst << "\n";
        }

        outs() << "  \t" << std::string(50, '-') << "\n";

        const auto res = analysis.dataMap.at(&bb);
        for(const auto &j : res) {
            outs() << "\t" << j.first << " : " << j.second << "\n";
        }
    }
}

int main(int argc, char *argv[]) {
    cl::ParseCommandLineOptions(argc, argv);

    if(InputFilename.empty()) {
        errs() << "filename is expected";
        abort();
    }

    LLVMContext ctx;
    SMDiagnostic diag;

    auto mod = parseIRFile(InputFilename, diag, ctx);

    if(auto msg = diag.getMessage(); !msg.empty()) {
        errs() << msg;
        abort();
    }

    const auto& funcList = mod->getFunctionList();

    for(const auto& func : funcList) {
        analyze(&func, MaxIteration);
    }
}
