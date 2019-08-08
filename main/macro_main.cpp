#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include "stool/src/cmdline.h"
#include "stool/src/io.h"
#include "stool/src/sa_bwt_lcp.hpp"
#include "../src/verification_attractor.hpp"
#include "esaxx/src/minimal_substrings/minimal_substring_iterator.hpp"

using namespace std;
using namespace sdsl;
using namespace stool;


uint64_t getFactorSize(uint64_t textSize, uint64_t edgeSize){
    if(textSize == 0) return 0;
    if(textSize < (edgeSize * 2) ){
        if(textSize <= edgeSize){
        return 1;
        }else{
        return 2;
        }
    }else{
        uint64_t remainingSize = textSize - (edgeSize * 2);
        return 2 + getFactorSize(remainingSize, edgeSize * 2);
    }
}

int main(int argc, char *argv[])
{
    using CHAR = uint8_t;
    using INDEX = uint64_t;

#ifdef DEBUG
    std::cout << "\033[41m";
    std::cout << "DEBUG MODE!";
    std::cout << "\e[m" << std::endl;
//std::cout << "\033[30m" << std::endl;
#endif
    cmdline::parser p;

    p.add<string>("input_file", 'i', "Input text file name", true);
    p.add<string>("attractor_file", 'a', "Attractors file name", true);
    p.add<string>("output_file", 'o', "(option) Error log file name (the default output name is 'input_file.verify.log')", false, "");

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    string attractorFile = p.get<string>("attractor_file");
    //string attractorFileType = p.get<string>("attractor_file_type");

    string outputFile = p.get<string>("output_file");


    // Loading Input Text
    std::ifstream ifs(inputFile);
    bool inputFileExist = ifs.is_open();
    if (!inputFileExist)
    {
        std::cout << inputFile << " cannot open." << std::endl;
        return -1;
    }

    vector<CHAR> text = stool::load_text_from_file(inputFile, true);

    // Loading Attractor File
    vector<uint64_t> attractors;
    stool::lazy::loadAttractorFile(attractorFile, attractors);

    uint64_t total = 0;
    for(uint64_t i=1;i<attractors.size();i++){
        uint64_t width = attractors[i] - attractors[i-1] - 1;
        //uint64_t w = 1 + getFactorSize(width, 1);
        total += getFactorSize(width, 1);
        total += 1; 
    }
    total += 1;
    total += getFactorSize(attractors[0] + (text.size() - attractors[attractors.size()-1]) - 1, 1);



    std::cout << "\033[36m";
    std::cout << "=============RESULT===============" << std::endl;
    std::cout << "File : " << inputFile << std::endl;
    std::cout << "Attractor File : " << attractorFile << std::endl;
    std::cout << "The length of the input text (with last special marker) : " << text.size() << std::endl;
    std::cout << "The number of attractors : " << attractors.size() << std::endl;
    std::cout << "MS size : " << total << std::endl;

    std::cout << "==================================" << std::endl;
    std::cout << "\033[39m" << std::endl;

    return 0;
}
