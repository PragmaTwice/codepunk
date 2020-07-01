//
// Created by edboy on 2020/7/1.
//

#ifndef CODEPUNK_INTERVALANALYSIS_H
#define CODEPUNK_INTERVALANALYSIS_H

#include <IntervalSolver.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/CFG.h>

#include <map>
#include <queue>

struct IntervalAnalysis {
    using Symbols = IntervalSymbols<const llvm::Value*>;

    std::map<const llvm::BasicBlock*, Symbols> dataMap;
    std::queue<const llvm::BasicBlock*> workList;

    explicit IntervalAnalysis(llvm::Function* f) {
        for(const auto &bb : f->getBasicBlockList()) {
            dataMap.emplace(&bb, Symbols{});
            workList.push(&bb);
        }

        auto entrySymbols = dataMap.at(&f->getEntryBlock());
        for(const auto& i : f->args()) {
            auto ty = i.getType();
            if(ty->isIntegerTy()) {
                entrySymbols.emplace((llvm::Value*)&i, Interval(
                        APSInt::getMinValue(ty->getIntegerBitWidth(), false),
                        APSInt::getMaxValue(ty->getIntegerBitWidth(), false)));
            }
        }
    }

    void analyze(int maxIteration = -1) {
        while(!workList.empty() && maxIteration != 0) {
            iterate();
            maxIteration--;
        }
    }

    void iterate() {
        auto bb = workList.front();
        workList.pop();

        auto oldSymbols = dataMap.at(bb);

        auto mergedSymbols = merge(std::vector<const llvm::BasicBlock*>{
                llvm::pred_begin(bb), llvm::pred_end(bb)});

        auto newSymbols = transfer(bb, mergedSymbols);
        dataMap.at(bb) = newSymbols;

        if(newSymbols != oldSymbols) {
            for(auto succBb : llvm::successors(bb)) {
                workList.push(succBb);
            }
        }
    }

    static Symbols merge(const std::vector<const llvm::BasicBlock*>& vec) {

    }

    static Symbols transfer(const llvm::BasicBlock *bb, const Symbols& symbols) {

    }
};

#endif //CODEPUNK_INTERVALANALYSIS_H
