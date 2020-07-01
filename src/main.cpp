#include <string>
#include <llvm/Support/CommandLine.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "IntervalAnalysis.h"

using namespace llvm;

static cl::opt<std::string> InputFilename(cl::Positional, cl::desc("filename of LLVM IR input"));

void analyze(const Function& f) {

    outs() << f.getName() << ":\n";
    for(const auto& v : f.args()) {
        outs() << "  ";
        if(v.hasName()) {
            outs() << v.getName() << "<" << &v << ">";
        } else {
            outs() << &v;
        }
    }
    outs() << "\n";
    for(const auto& bb : f.getBasicBlockList()) {
        outs() << "  [";
        if(bb.hasName()) {
            outs() << bb.getName() << "<" << &bb << ">";
        } else {
            outs() << &bb;
        }
        outs() << "]\n";

        for(const auto& inst : bb.getInstList()) {
            outs() << '\t' << inst.getOpcodeName() << " (";
            bool isF = true;
            for(const auto& v : inst.operands()) {
                if(isF) isF = false;
                else outs() << ", ";

                if(v->hasName()) {
                    outs() << v->getName() << "<" << v << ">";
                } else if(auto c = dyn_cast<Constant>(v)) {
                    outs() << c->getUniqueInteger();
                } else {
                    outs() << v;
                }
            }
            outs() << ") -> ";

            const auto ret = &cast<Value>(inst);
            if(ret->hasName()) {
                outs() << ret->getName() << "<" << ret << ">";
            } else {
                outs() << ret;
            }
            outs() << "\n";
        }
    }

    outs() << "-----analysis-----\n";

    IntervalAnalysis analysis(&f);
    analysis.analyze(999);

    for(const auto &i : analysis.dataMap) {
        outs() << "  [" << i.first->getName() << "]\n";
        for(const auto &j : i.second) {
            outs() << "\t";
            if(j.first->hasName()) {
                outs() << j.first->getName();
            } else {
                outs() << j.first;
            }
            outs() << " : " << j.second << "\n";
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
        analyze(func);
    }
}
