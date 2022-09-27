#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "../include/cf.h"
#include "../include/encode.h"

int main() {

    std::ofstream output("../data/output.txt");
    std::ofstream input_binary("../data/input_binary.txt");
    std::ifstream f("../data/input.txt");
    std::stringstream buffer;
    buffer << f.rdbuf(); std::string input = buffer.str() + ']'; // end character, for encoding

    std::cout << std::fixed << std::setprecision(2) << std::endl;

    try {
        std::string encoded = encode(input);
        output << encoded; input_binary << encoded;
    } catch (const char *err) {
        output.close();
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    output.close();

}