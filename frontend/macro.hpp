//
// Created by tcn on 22-5-29.
//

#ifndef COMPILER_MACRO_HPP
#define COMPILER_MACRO_HPP
#include <unordered_map>
#include <cstring>
#include <vector>
#include <regex>
#include <iostream>

using namespace std;
class macro {
    unordered_map<string, string> macro_map;
    unordered_map<string, vector<string>> macro_var;
public:
    bool add_macro(const string& a, const string& b);
    bool add_macro(const string& a, const string& b, string var_list);
    int function(const string& a);
    string get_macro(const string& a);
    string get_macro(const string& a, string var_list);
};


#endif //COMPILER_MACRO_HPP
