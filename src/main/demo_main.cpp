// License: MIT http://opensource.org/licenses/MIT

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "stool/src/cmdline.h"
#include "divsufsort.h"
#include "divsufsort64.h"
#include "stool/src/io.hpp"
#include "stool/src/sa_bwt_lcp.hpp"
#include "libdivsufsort/sa.hpp"
#include "esaxx/src/main/common.hpp"
#include "../src/common.hpp"
#include "../greedy/position_frequency_set.hpp"
#include "../lazy/lazy_attractor.hpp"

using namespace std;
using namespace stool;

int main(int argc, char *argv[])
{

    cmdline::parser p;
    p.add<string>("input_file", 'i', "input file name", true);
    p.add<std::string>("msubstr_file", 'm', "(option) The file name of minimal substrings for the input text file(the default minimal substrings filename is 'input_file.msub')", false, "");
    p.add<uint64_t>("k-attr", 'k', "(option) the value of k-attractor", false, 0);
    p.add<string>("attractor algorithm", 'a', "The attractor algorithm for demo(lazy or greedy)", true);

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    std::string mSubstrFile = p.get<std::string>("msubstr_file");
    uint64_t k_attr = p.get<uint64_t>("k-attr");
    string algorithm = p.get<string>("attractor algorithm");

    std::vector<uint8_t> T = stool::load_text_from_file(inputFile, true); // input text
    std::vector<uint64_t> sa = stool::construct_suffix_array(T);

    // Loading Minimal Substrings
    std::vector<stool::LCPInterval<uint64_t>> minimalSubstrings = stool::lazy::loadOrConstructMS(mSubstrFile, T, sa, k_attr);
    uint64_t mSubstrCount = minimalSubstrings.size();

    std::vector<uint64_t> attrs;
    uint64_t round = 0;
    while (minimalSubstrings.size() > 0)
    {
        std::cout << "============ ROUND " << round++ << " =============" << std::endl;
        uint64_t nextLazyAttractor = stool::lazy::print_info(T, sa, minimalSubstrings, attrs, algorithm);
        attrs.push_back(nextLazyAttractor);
        minimalSubstrings = stool::lazy::filter(minimalSubstrings, sa, attrs);
        std::cout << std::endl;
    }

    std::sort(attrs.begin(), attrs.end());



    std::cout << "\033[36m";
    std::cout << "============= INFO ===============" << std::endl;
    std::cout << "File : " << inputFile << std::endl;
    std::cout << "The input file content: ";
    if (T.size() <= 100)
    {
        std::vector<char> s;
        stool::load_vector(inputFile, s, false, false);
        for (auto &c : s)
            std::cout << c;
        std::cout << std::endl;
    }
    else
    {
        std::cout << "(we omit to print it on the console if its size is larger than 100)" << std::endl;
    }

    std::cout << "The length of the input text (with last special marker): " << T.size() << std::endl;
    std::cout << "Attractor algorithm: " << algorithm << std::endl;
    
    std::cout << "\033[33m";
    std::cout << "=============RESULT===============" << std::endl;
    std::cout << "The number of minimal substrings: " << mSubstrCount << std::endl;
    std::cout << "The number of attractors: " << attrs.size() << std::endl;
    std::cout << "The obtained attractors: ";
    if(attrs.size() <= 100){
        stool::Printer::print(attrs);
    }else{
        std::cout << "(we omit to print it on the console if its size is larger than 100)" << std::endl;
    }
    std::cout << "==================================" << std::endl;
    std::cout << "\033[39m" << std::endl;

    return 0;
}