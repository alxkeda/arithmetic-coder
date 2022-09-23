#include "cf.h"

Symbol::Symbol() {
    frequency = 1;
}

std::unordered_map<char, Symbol> Metadata::mk_cf_table(std::string s) { // iterates through ordered map for consistent decoding and encoding tables without the need to store extra metadata

    std::map<char, Symbol> metadata = Metadata::collect_ordered_freq(s);

    uint32_t low = 0;
    std::unordered_map<char, Symbol> cf_table;

    for(std::pair<char, Symbol> character : metadata) {
        char curr = character.first;
        uint32_t freq = character.second.frequency;
        cf_table[curr].frequency = freq;
        cf_table[curr].low = low;
        cf_table[curr].high = low + freq;
        low += freq;
    }

    return cf_table;

}

void Metadata::print_table(std::unordered_map<char, Symbol> metadata) { // prints tables for debugging
    for(std::unordered_map<char, Symbol>::const_iterator character = metadata.begin(); character != metadata.end(); ++character) {
        std::cout << character->first << ":\n" << "{ frequency: " << character->second.frequency << std::endl << "lower bound: " << character->second.low << std::endl << "higher bound: " << character->second.high << std::endl << " }" << std::endl;
    }
}

std::map<char, Symbol> Metadata::collect_ordered_freq(std::string sequence) { // collects counts of each symbol
    std::map<char, Symbol> metadata;
    for(char character : sequence) {
        if(metadata.find(character) == metadata.end()) {
            metadata[character] = Symbol();
        } else {
            metadata[character].frequency++;
        }
    }
    return metadata;
}
