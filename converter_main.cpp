#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include "cmdline.h"
#include "stool/src/io.h"
#include "sa_lcp.hpp"
#include "verification_attractor.hpp"

using namespace std;
using namespace sdsl;
using namespace stool;

int main(int argc, char *argv[])
{
    #ifdef DEBUG
    std::cout << "\033[41m";
    std::cout << "DEBUG MODE!";
    std::cout << "\e[m" << std::endl;
    //std::cout << "\033[30m" << std::endl;
    #endif
    cmdline::parser p;
    p.add<string>("input_file", 'i', "input file name", true);
    p.add<string>("output_file", 'o', "output file name (the default output name is 'input_file.ext')", false, "");

    p.parse_check(argc, argv);
    std::string inputFile = p.get<string>("input_file");
    std::string outputFile = p.get<string>("output_file");

    if (outputFile.size() == 0)
    {
        outputFile = inputFile + ".attrs";
    }
    std::ifstream ifs(inputFile);
    bool inputFileExist = ifs.is_open();
    if(!inputFileExist){
        std::cout << inputFile << " cannot open." << std::endl;
        return -1;
    }

    std::vector<std::pair<uint64_t,uint64_t>> pairs;
    IO::load(inputFile, pairs);

    std::vector<uint64_t> attrs;

    uint64_t x=0;
    for(auto& p : pairs){
        attrs.push_back(x);
        if(p.second == UINT64_MAX){
            x+=1;
        }else{
            x+= p.second;
        }
    }
    IO::write(outputFile, attrs);
    std::cout << "wrote" << std::endl;

    
    return 0;

    
}
