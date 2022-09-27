#include "../include/cf.h"

Symbol::Symbol() : frequency(1) {}
Symbol::Symbol(uint32_t high, uint32_t low) : high(high), low(low) {}

std::unordered_map<char, Symbol> Metadata::mk_cf_table(std::string s) {

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

uint32_t Metadata::count_cf_freq(std::unordered_map<char, Symbol> metadata) {
    uint32_t count = 0;
    for(std::unordered_map<char, Symbol>::const_iterator iter = metadata.begin(); iter != metadata.end(); ++iter) {
        count += iter->second.frequency;
    }    
    return count;
}

uint32_t Metadata::count_cf_freq(std::unordered_map<char, Symbol> metadata, bool is_decoding) {
    uint32_t count = 0;
    for(std::unordered_map<char, Symbol>::const_iterator iter = metadata.begin(); iter != metadata.end(); ++iter) {
        count += (iter->second.high - iter->second.low);
    }    
    return count;
}


void Metadata::print_table(std::unordered_map<char, Symbol> metadata) {
    for(std::unordered_map<char, Symbol>::const_iterator iter = metadata.begin(); iter != metadata.end(); ++iter) {
        std::cout << iter->first << ":\n" << "{ frequency: " << iter->second.frequency << std::endl << "lower bound: " << iter->second.low << std::endl << "higher bound: " << iter->second.high << std::endl << " }" << std::endl;
    }
}