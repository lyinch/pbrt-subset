#include <iostream>
#include <vector>
#include "api.h"
#include "parser.h"

using namespace pbrt;

int main(int argc, char *argv []) {
    /*
    std::vector<std::string> filenames;

    // don't support any command-line arguments except for file names
    for (int i = 1; i < argc; ++i) {
        filenames.emplace_back(argv[i]);
    }

    if(filenames.empty()){
        // require at least one file. Don't support reading from stdin
        std::cout << "No filename was given!" << std::endl;
        return -1;
    }else{
        pbrtParseFile(filenames.at(0));
    }*/

    Options options;
    pbrtInit(options);

    parse();

    pbrtCleanup();



    return 0;
}

