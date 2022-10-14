#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <processthreadsapi.h>

#include "../include/cf.h"
#include "../include/encode.h"

// const double ENCODING = ;
// provide the original encoding size above then compile

double retrieve_cpu_time(){
    FILETIME a, b, c, d;
    if(GetProcessTimes(GetCurrentProcess(), &a, &b, &c, &d) != 0) {
        return
            (double)(d.dwLowDateTime |
            ((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
    } else {
        return 1;
    }
}

int main() {

    std::ofstream output("../data/output.txt");
    std::ofstream input_binary("../data/input_binary.txt");
    std::ifstream f("../data/input.txt");
    std::stringstream buffer;
    buffer << f.rdbuf(); std::string input = buffer.str() + ']'; // end character, for encoding

    std::cout << std::fixed << std::setprecision(8) << std::endl;

    try {
        double start = retrieve_cpu_time();
        std::string encoded = encode(input);
        double end = retrieve_cpu_time();
        std::cout << (double)input.length() * ENCODING / (double)encoded.find(']') << std::endl << end - start << std::endl;
        output << encoded; input_binary << encoded;
    } catch (const char *err) {
        output.close();
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    output.close();

}