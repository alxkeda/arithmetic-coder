#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <bitset>
#include <cmath>
#include <iomanip>

#include "../include/cf.h"

char assign_bit(bool bit);
void shift_bounds(std::bitset<32>* h, std::bitset<32>* l);
std::string create_table(std::unordered_map<char, Symbol> metadata);
std::string encode(std::string sequence, std::unordered_map<char, Symbol> metadata);