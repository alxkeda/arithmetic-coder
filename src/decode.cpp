#include <string>
#include <unordered_map>
#include <bitset>
#include <cmath>

#include "../include/decode.h"
#include "../include/cf.h"

void shift_bounds(uint32_t* h, uint32_t* l) {
    *h = *h << 1; *h += 1;
    *l = *l << 1; *l += 0;
}

void shift_bound(uint32_t* w) {
    *w = *w << 1; *w += 1;
}

std::unordered_map<char, Symbol> create_table(std::string sequence) {
    std::unordered_map<char, Symbol> metadata;
    int i = sequence.find(']') + 1;
    while(i < sequence.length()) {
        char c = sequence[i];
        if(!isdigit(sequence[i])) {
            int d = 1;
            while(isdigit(sequence[i + d])) {
                d++;
            }
            int high = std::stoi(sequence.substr(i + 1, d));
            i += d;
            
            d = 1;
            while(isdigit(sequence[i + d])) { // repeats
                d++;
            }
            int low = std::stoi(sequence.substr(i + 1, d));
            i += d;

            metadata[c] = Symbol(high, low);
        }
    }
    return metadata;
}

std::string decode(std::string sequence) {

    std::string* s = &sequence;

    uint32_t num_decoded = 0;
    std::string output;
    char c;
    bool index_err = 1;

    std::unordered_map<char, Symbol> metadata = create_table(sequence);

    const int MSB = pow(2, 5) - 1;
    
    uint32_t high = pow(2, 32) - 2;
    uint32_t low = 0;
    long double range;
    long double character_range;

    uint32_t cf = Metadata::count_cf_freq(metadata, true);
    int32_t len = sequence.find(']');

    uint32_t window;
    int num = 32 - len;
    int32_t i;
    if(num > 0) {
        i = len - 32;
    } else {
        i = 0;
    }

    while(true) {

        for(int n=0; n<num; n++) {
            (*s).insert(sequence.find("]"), "1");
            i += 1;
        }
        num = 0;

        window = std::stoul(sequence.substr(i, 32), nullptr, 2);
        character_range = (long double)(((uint64_t)cf * (uint64_t)(window - low + 1) - 1) / (long double)((high + 1) - low));

        for(std::unordered_map<char, Symbol>::const_iterator iter = metadata.begin(); iter != metadata.end(); ++iter) {
            c = iter->first;
            if((long double)metadata.at(c).high > character_range && (long double)metadata.at(c).low < character_range) {
                index_err = 0;
                break;
            }
        } if(index_err) {
            throw std::logic_error("The encoding is out of bounds");
        } else {
            index_err = 1;
        } if(c == ']') {
            std::cout << std::endl << "Encoding finished." << "\n\n";
            break;
        } else {
            output.push_back(c);
        }

        range = ((long double)high + 1.0) - (long double)low;
        high = low + (range * ((long double)metadata.at(c).high / (long double)cf));
        low = low + (range * ((long double)metadata.at(c).low / (long double)cf));
        
        if(high <= low) {
            std::cout << "\rDecoding failed at \"" << sequence[i] << "\"" << ". Percent encoded: " << 100 * (long double)num_decoded / len << std::endl;
            throw std::logic_error("Upper and lower bounds crossed.\n");
        }

        while(true) {
            std::bitset<32> high_bits(high);
            std::bitset<32> low_bits(low);
            std::bitset<32> window_bits(window);
            if(high_bits[MSB] == low_bits[MSB]) {
                shift_bounds(&high, &low);
                shift_bound(&window); 
                num += 1;
            } else if(high_bits[MSB - 1] == 0 && low_bits[MSB - 1] == 1) {
                high_bits = (high_bits << 2) >> 1; high_bits.set(MSB, 1); high = high_bits.to_ulong() + 1;
                low_bits = (low_bits << 2) >> 1; low_bits.set(MSB, 0); low = low_bits.to_ulong() + 0;
                bool window_temp = window_bits[MSB];
                window_bits = (window_bits << 2) >> 1; window_bits.set(MSB, window_temp); window = window_bits.to_ulong() + 1;
                (*s).erase(i + num + 1, 1); (*s).insert(sequence.find(']'), "1");
                // num += 1;
            } else {
                break;
            }
        }

        std::cout << "\rProgress: " << 100.0 * ((long double)i + 32.0) / len << "%";

    }

    return output;
}