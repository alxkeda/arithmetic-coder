#include "encode.h"

// std::bitset<64> cast_uint64_bitset64(uint32_t input) {
//     return output;
// }

char assign_bit(bool bit) {
    char c; if(bit == 0) { c = '0'; } else { c = '1'; }
    return c;
}

void check_underflow(std::bitset<32>* h, std::bitset<32>* l, int* underflow) {
    int msb = (*h).size() - 1; int smsb = (*h).size() - 2;
    while((*h)[msb] != (*h)[smsb] && (*l)[msb] != (*l)[smsb] && (*h)[msb] != (*l)[msb]) {
        bool htemp = (*h)[msb]; bool ltemp = (*l)[msb];
        (*h) = ((*h) << 2) >> 1;
        (*l) = ((*l) << 2) >> 1;
        (*h).set(msb, htemp); (*h).set(0, 1); (*l).set(msb, ltemp); (*l).set(0, 0); // l.set not necessary
        *underflow++;
    }
}

void shift_bounds(std::bitset<32>* h, std::bitset<32>* l) {
    *h = *h << 1; (*h).set(0, 1);
    *l = *l << 1; (*l).set(0, 0);     
}

void shift_output(uint32_t* high, uint32_t* low, std::string* output, int* underflow) {
    std::bitset<32> h(*high);
    std::bitset<32> l(*low);
    int msb = h.size() - 1;

    if(h[msb] == l[msb] && *underflow > 0) { // deals with underflow, if any
        char bit = assign_bit(h[msb]); char bit_neg = assign_bit(!h[msb]);
        (*output).push_back(bit);
        shift_bounds(&h, &l);      
        for(int i=0; i<*underflow; i++) {
            (*output).push_back(bit_neg);
        }
        *underflow = 0;
    }

    while(h[msb] == l[msb]) { // outputs all matching bits
        char bit = assign_bit(h[msb]);
        (*output).push_back(bit);
        shift_bounds(&h, &l);
    }

    check_underflow(&h, &l, underflow); // updates underflow counter
    *high = h.to_ulong(); *low = l.to_ulong();
}

void end(uint32_t low, std::string* output) { // is called when the end character is read, then creates the shortest binary sequence which will place our output between the two current ranges
    (*output).push_back('0');
    
    std::bitset<32> l(low);
    uint32_t i = l.size();
    while(l[i] != 0) {
        i--;
        (*output).push_back('1');
    }
}

std::string encode(std::string sequence, std::unordered_map<char, Symbol> metadata) {

    std::cout << std::fixed << std::setprecision(2) << std::endl; // sets output to two decimal precision for progress indicator

    uint32_t num_encoded = 0;
    std::string output;

    uint32_t high = pow(2, 32) - 1;
    uint32_t low = 0;
    uint32_t range;

    int underflow = 0;
    uint32_t len = sequence.length();

    for(char character : sequence) {

        range = high - low;
        high = low + (
            (long double)range * ((long double)metadata.at(character).high / (long double)len)
        );
        low = low + (
            (long double)range * ((long double)metadata.at(character).low / (long double)len)
        );
        
        if(high <= low) {
            std::cout << "\rEncoding failed at \"" << character << "\"" << ". Percent encoded: " << 100 * (long double)num_encoded / len << std::endl;
            throw std::logic_error("Upper and lower bounds crossed.\n");
        }

        shift_output(&high, &low, &output, &underflow);

        num_encoded++;
        std::cout << "\rProgress: " << 100 * (long double)num_encoded / len << "%";

        if(character == ']') {
            std::cout << std::endl << "Encoding finished." << "\n\n";
            end(low, &output);
        }



    }

    return output;

}