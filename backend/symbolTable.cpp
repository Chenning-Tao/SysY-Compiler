//
// Created by tcn on 22-4-27.
//

#include "symbolTable.hpp"

void symbolTable::insert(const std::string& name, AllocaInst *input) {
    auto re = table.find(name);
    // if not in symbol table
    if(re == table.end()){
        auto new_stack = std::stack<AllocaInst*>();
        new_stack.push(input);
        table.emplace(name, new_stack);
    }
    // already have definition in symbol table
    else re->second.push(input);
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
