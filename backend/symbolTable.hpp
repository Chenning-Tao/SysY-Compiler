//
// Created by tcn on 22-4-27.
//

#ifndef COMPILER_SYMBOLTABLE_HPP
#define COMPILER_SYMBOLTABLE_HPP
#include <unordered_map>
#include <string>
#include <stack>
#include <vector>
#include "llvm/IR/Instructions.h"

using namespace llvm;

class symbolTable {
private:
    std::unordered_map<std::string, std::stack<AllocaInst *>> table;
public:
    void insert(const std::string& name, AllocaInst* input);
    void remove(const std::vector<std::string>& name);
    AllocaInst* find(const std::string& name);
};


#endif //COMPILER_SYMBOLTABLE_HPP
