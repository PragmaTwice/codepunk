#include <string>
#include <llvm/Support/CommandLine.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "IntervalSolver.h"

using namespace llvm;

static cl::opt<std::string> InputFilename(cl::Positional, cl::desc("filename of LLVM IR input"));

void analyze(const Function& f) {

    outs() << f.getName() << ":\n";
    for(const auto& bb : f.getBasicBlockList()) {
        outs() << "  [";
        if(bb.hasName()) {
            outs() << bb.getName();
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
                    outs() << v->getName();
                } else if(auto c = dyn_cast<Constant>(v)) {
                    outs() << c->getUniqueInteger();
                } else {
                    outs() << v;
                }
            }
            outs() << ") -> ";

            const auto ret = &cast<Value>(inst);
            if(ret->hasName()) {
                outs() << ret->getName();
            } else {
                outs() << ret;
            }
            outs() << "\n";
        }
    }

    APSInt a("3"), b("9"), c("12"), d("14");
    IntervalSolver<std::string> solver{std::make_shared<std::map<std::string, Interval>>(
            std::map<std::string, Interval>{{"a", Interval(a,d)},
                                            {"9", Interval(b)},
                                            {"12", Interval(c)}}),
            std::make_shared<AndOp<std::string>>(
                    std::make_shared<BinOp<std::string>>(BinOp<std::string>::LT,
                            std::make_shared<Atom<std::string>>("a"),
                            std::make_shared<Atom<std::string>>(("12"))
                    ),
                    std::make_shared<BinOp<std::string>>(BinOp<std::string>::GT,
                                            std::make_shared<Atom<std::string>>("a"),
                                            std::make_shared<Atom<std::string>>(("9"))
                    )
            )
    };

    for(const auto& i : solver.solve(true)) {
        outs() << i.first << ":" << i.second << "\n";
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
