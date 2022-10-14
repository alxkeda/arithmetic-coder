#include <iostream>
#include <string>
#include <unordered_map>
#include <bitset>
#include <cmath>

#include "../include/encode.h"
#include "../include/cf.h"

char assign_bit(bool bit) {
    char c; if(bit == 0) { c = '0'; } else { c = '1'; }
    return c;
}

void shift_bounds(uint32_t* h, uint32_t* l) {
    *h = *h << 1; *h += 1;
    *l = *l << 1; *l += 0;     
}

std::string create_table(std::unordered_map<char, Symbol> metadata) {
    std::string table = "]";
    for(std::unordered_map<char, Symbol>::const_iterator iter = metadata.begin(); iter != metadata.end(); ++iter) {
        table.push_back(iter->first);
        table.push_back(' ');
        table.append(std::to_string(iter->second.high));
        table.push_back(' ');
        table.push_back(iter->first);
        table.push_back(' ');
        table.append(std::to_string(iter->second.low));
        table.push_back(' ');
    }
    return table;
}

void end(uint32_t low, std::string* output) { // is called when the end character is read, then creates the shortest binary sequence which will place our output between the two current ranges
    std::bitset<32> l(low);
    (*output).append("01");
    int i = l.size() - 3;
    while(l[i] != 0) {
        (*output).push_back('1');
        i--;
    }
}

std::string encode(std::string sequence) {

    // uint32_t num_encoded = 0;
    std::string output;

    std::unordered_map<char, Symbol> metadata = Metadata::mk_cf_table(sequence);

    const int MSB = pow(2, 5) - 1;

    uint32_t high = pow(2, 32) - 1;
    uint32_t low = 0;
    long double range;
    int underflow = 0;

    uint32_t cf = Metadata::count_cf_freq(metadata);
    uint32_t len = sequence.length(); // cf = len for encoding

    for(char character : sequence) {

        range = ((long double)high + 1.0) - (long double)low;
        high = low + ((long double)range * ((long double)metadata.at(character).high / (long double)cf));
        low = low + ((long double)range * ((long double)metadata.at(character).low / (long double)cf));
        
        if(high <= low) {
            // << ". Percent encoded: " << 100 * (long double)num_encoded / len
            std::cout << "\rEncoding failed at \"" << character << "\"" << std::endl;
            throw std::logic_error("Upper and lower bounds crossed.\n");
        }

        while(true) {
            std::bitset<32> high_bits(high);
            std::bitset<32> low_bits(low);
            if(high_bits[MSB] == low_bits[MSB]) {
                output.push_back(assign_bit(high_bits[MSB]));
                shift_bounds(&high, &low);
                int uf = underflow;
                for(int u=0; u<uf; u++) {
                    output.push_back(assign_bit(!high_bits[MSB]));
                    underflow = 0;
                }
            } else if(high_bits[MSB - 1] == 0 && low_bits[MSB - 1] == 1) {
                high_bits = (high_bits << 2) >> 1; high_bits.set(MSB, 1); high = high_bits.to_ulong() + 1;
                low_bits = (low_bits << 2) >> 1; low_bits.set(MSB, 0); low = low_bits.to_ulong() + 0;
                underflow += 1;
            } else {
                break;
            }
        }

        // num_encoded++;
        // std::cout << "\rProgress: " << 100 * (long double)num_encoded / len << "%";

    }

    end(low, &output);
    output.append(create_table(metadata));

    return output;

}