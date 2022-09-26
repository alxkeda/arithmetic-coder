#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>

#include "../include/cf.h"
#include "../include/decode.h"

int main() {

    std::ofstream output("../data/output.txt");
    std::ifstream f("../data/input_binary.txt");
    std::stringstream buffer;
    buffer << f.rdbuf(); std::string input = buffer.str();

    try {
        std::string decoded = decode(input);
        output << decoded;
    } catch (const char *err) {
        output.close();
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    output.close();

}