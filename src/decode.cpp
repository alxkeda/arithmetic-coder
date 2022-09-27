#include <string>
#include <unordered_map>
#include <bitset>
#include <cmath>
#include <iomanip>

#include "../include/decode.h"
#include "../include/cf.h"

void shift_bounds(std::bitset<32>* h, std::bitset<32>* l) {
    *h = *h << 1; (*h).set(0, 1);
    *l = *l << 1; (*l).set(0, 0);     
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

void check_underflow(std::bitset<32>* h, std::bitset<32>* l) {
    int msb = (*h).size() - 1; int smsb = (*h).size() - 2;
    while((*h)[msb] != (*h)[smsb] && (*l)[msb] != (*l)[smsb] && (*h)[msb] != (*l)[msb]) {
        bool htemp = (*h)[msb]; bool ltemp = (*l)[msb];
        (*h) = ((*h) << 2) >> 1;
        (*l) = ((*l) << 2) >> 1;
        (*h).set(msb, htemp); (*h).set(0, 1); (*l).set(msb, ltemp); (*l).set(0, 0); // l.set not necessary
    }
}

int shift_output(uint64_t* high, uint32_t* low) {
    int num = 0;
    std::bitset<32> h(*high);
    std::bitset<32> l(*low);
    int msb = h.size() - 1;

    while(h[msb] == l[msb]) { // outputs all matching bits
        shift_bounds(&h, &l);
        num++;
    }

    check_underflow(&h, &l); // updates underflow counter
    *high = h.to_ulong(); *low = l.to_ulong();

    return num;
}

std::string decode(std::string sequence) {

    std::string* s = &sequence;

    std::cout << std::fixed << std::setprecision(2) << std::endl; // sets output to two decimal precision for progress indicator

    uint32_t num_decoded = 0;
    std::string output;
    char c; bool index_err = 1;
    std::unordered_map<char, Symbol> metadata = create_table(sequence);

    int32_t i = 0;
    uint32_t window;
    
    uint64_t high = pow(2, 32) - 1;
    uint32_t low = 0;
    uint64_t range;
    long double character_range;

    uint32_t cf = Metadata::count_cf_freq(metadata, true);
    int32_t decode_len = sequence.find(']');

    if(i >= decode_len - 32) { // deals with the case of very small encodings
        int num = 32 - decode_len;
        for(int i=0; i<num; i++) {
            (*s).insert(decode_len, "1");
        }
        decode_len = 32;
    }

    while(true) {

        int num = 0;

        if(i > decode_len) {
            throw std::logic_error("Encoding failed");
        } else {
            window = std::stoul(sequence.substr(i, 32), nullptr, 2);
        }

        character_range = (long double)(((uint64_t)cf * (uint64_t)(window - low + 1) - 1.0) / (long double)((high + 1) - low));

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

        range = high - low;
        high = low + (
            (long double)range * ((long double)metadata.at(c).high / (long double)cf)
        );
        low = low + (
            (long double)range * ((long double)metadata.at(c).low / (long double)cf)
        );

        if(high <= low) {
            std::cout << "\rDecoding failed at \"" << sequence[i] << "\"" << ". Percent encoded: " << 100 * (long double)num_decoded / decode_len << std::endl;
            throw std::logic_error("Upper and lower bounds crossed.\n");
        }

        num = shift_output(&high, &low);

        i += num; num_decoded += num;

        for(int i=0; i<num; i++) {
            (*s).insert(sequence.find("]"), "1");
        }

        std::cout << "\rProgress: " << 100 * (long double)num_decoded / decode_len << "%";

    }

    return output;
}