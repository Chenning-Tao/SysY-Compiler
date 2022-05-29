//
// Created by tcn on 22-5-29.
//

#include "macro.hpp"

bool macro::add_macro(const string& a, const string& b) {
    if (a.length() == 0 || b.length() == 0) return false;
    if (macro_map.find(a) != macro_map.end()) return false;
    macro_map[a] = b;
    return true;
}

bool macro::add_macro(const string& a, const string& b, string var_list){
    if (a.length() == 0 || b.length() == 0) return false;
    if (macro_map.find(a) != macro_map.end()) return false;
    macro_map[a] = b;
    regex delimiters("[ ]*,[ ]*");
    vector<string> v(sregex_token_iterator(var_list.begin(),var_list.end(),delimiters,-1),
                                sregex_token_iterator());
    macro_var[a] = v;
    return true;
}

int macro::function(const string& a) {
    if (macro_map.find(a) == macro_map.end()) return 0;
    else if (macro_var[a].empty()) return 1;
    else return 2;
}

string macro::get_macro(const string& a) {
    return macro_map[a];
}

string macro::get_macro(const string& a, string var_list) {
    regex delimiters("[ ]*,[ ]*");
    vector<string> v(sregex_token_iterator(var_list.begin(),var_list.end(),delimiters,-1),
                     sregex_token_iterator());
    vector<string> var = macro_var[a];
    string expand = macro_map[a];
    for(int i = 0; i < var.size(); ++i){
        size_t pos = expand.find(var[i]);
        while(pos != string::npos){
            expand.replace(pos, v[i].size(), v[i]);
            pos = expand.find(var[i]);
        }
    }
    return expand;
}
