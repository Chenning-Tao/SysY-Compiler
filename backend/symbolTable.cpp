//
// Created by tcn on 22-4-27.
//

#include "symbolTable.hpp"

void symbolTable::insert(const std::string& name, AllocaInst *input, std::vector<std::shared_ptr<BaseAST>>& dim) {
    auto re = table.find(name);
    auto ar = array.find(name);
    // if not in symbol table
    if(re == table.end()){
        // add variable
        auto new_stack = std::stack<AllocaInst*>();
        new_stack.push(input);
        table.emplace(name, new_stack);
        // add array length
        auto array_stack = std::stack<std::vector<std::shared_ptr<BaseAST>>>();
        array_stack.push(dim);
        array.emplace(name, array_stack);
    }
    // already have definition in symbol table
    else {
        re->second.push(input);
        ar->second.push(dim);
    }
}

void symbolTable::remove(const std::vector<std::string>& name) {
    for(auto & i : name){
        auto re = table.find(i);
        // TODO: identify possible error
        if (re == table.end()) continue;
        if (re->second.size() == 1) table.erase(re);
        else re->second.pop();
    }
}

AllocaInst *symbolTable::find(const std::string& name) {
    auto re = table.find(name);
    if (re == table.end()) return nullptr;
    return re->second.top();
}

vector<shared_ptr<BaseAST>> symbolTable::array_dim(const std::string &name) {
    auto re = array.find(name);
    if (re == array.end()) return {};
    return re->second.top();
}
