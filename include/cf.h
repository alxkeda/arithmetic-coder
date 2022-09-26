#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <map>

class Symbol {
    public:
        uint32_t frequency;
        uint32_t high;
        uint32_t low;
        Symbol();
        Symbol(uint32_t high, uint32_t low);
};

class Metadata {
    public:
        static std::unordered_map<char, Symbol> mk_cf_table(std::string s);
        static std::map<char, Symbol> collect_ordered_freq(std::string sequence);
        static void print_table(std::unordered_map<char, Symbol> metadata);
};