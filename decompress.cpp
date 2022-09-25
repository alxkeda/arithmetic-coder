#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>

#include "cf.h"
#include "encode.h"
#include "decode.h"

int main() {

    std::ofstream output("./data/output.txt");
    std::ifstream f("./data/input_binary.txt");
    std::stringstream buffer;
    buffer << f.rdbuf(); std::string input = buffer.str();

    std::unordered_map<char, Symbol> cf_table = Metadata::mk_cf_table(input);

    try {
        std::string decoded = decode(input, cf_table);
        output << decoded;
    } catch (const char *err) {
        output.close();
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    output.close();

}