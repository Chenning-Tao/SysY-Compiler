//
// Created by tcn on 22-5-29.
//

#ifndef COMPILER_MACRO_HPP
#define COMPILER_MACRO_HPP
#include <unordered_map>
#include <cstring>

using namespace std;
class macro {
    unordered_map<string, string> macro_map;
public:
    bool insert(string a, string b);
    string get(string a);
};


#endif //COMPILER_MACRO_HPP
