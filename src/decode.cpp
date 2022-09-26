#include <string>
#include <unordered_map>
#include <bitset>
#include <cmath>
#include <iomanip>

#include "../include/decode.h"
#include "../include/cf.h"

std::unordered_map<char, Symbol> create_table(std::string sequence) {
    std::unordered_map<char, Symbol> metadata;
    int i = sequence.find(']') + 1;
    while(i < sequence.length()) {
        if(!isdigit(sequence[1])) {
            int o = i;
            int d = 0;

            while(!isdigit(sequence[i + d])) {
                d++;
            }
            int high = std::stoi(sequence.substr(i + 1, d));
            i += d;

            while(!isdigit(sequence[i + d])) { // repeats
                d++;
            }
            int low = std::stoi(sequence.substr(i + 1, d));
            i += d;

            metadata[o] = Symbol(high, low);
        }
    }
    return metadata;
}

std::string decode(std::string sequence) {
    std::string output;
    std::unordered_map<char, Symbol> metadata = create_table(sequence);


    return output;
}