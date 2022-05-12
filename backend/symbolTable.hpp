//
// Created by tcn on 22-4-27.
//

#ifndef COMPILER_SYMBOLTABLE_HPP
#define COMPILER_SYMBOLTABLE_HPP
#include <unordered_map>
#include <string>
#include <stack>
#include <vector>
#include <memory>
#include "ast.hpp"
#include "llvm/IR/Instructions.h"

using namespace llvm;

class symbolTable {
private:
    std::unordered_map<std::string, std::stack<AllocaInst *>> table;
    // value indicate array dim, 0 indicate it's not an array
    std::unordered_map<std::string, std::stack<std::vector<std::shared_ptr<BaseAST>>>> array;
public:
    void insert(const std::string& name, AllocaInst* input, std::vector<std::shared_ptr<BaseAST>> &dim);
    void remove(const std::vector<std::string>& name);
    AllocaInst* find(const std::string& name);
    vector<shared_ptr<BaseAST>> array_dim(const std::string& name);
};


#endif //COMPILER_SYMBOLTABLE_HPP
