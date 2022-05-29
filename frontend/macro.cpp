//
// Created by tcn on 22-5-29.
//

#include "macro.hpp"

bool macro::insert(string a, string b) {
    if (a.length() == 0) {
        return false;
    }
    if (b.length() == 0) {
        return false;
    }
    if (macro_map.find(a) != macro_map.end()) {
        return false;
    }
    macro_map[a] = b;
    return true;
}

string macro::get(string a) {
    if (macro_map.find(a) == macro_map.end()) {
        return "";
    }
    return macro_map[a];
}