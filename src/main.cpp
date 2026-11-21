#include <iostream>
#include "ui.h"

int main(int argc, char** argv) {
    std::string csvPath = (argc > 1) ? argv[1] : "data/edge_table.csv";
    return runProgram(std::cin, std::cout, csvPath);
}
