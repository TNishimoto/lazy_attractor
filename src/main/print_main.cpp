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
#include "../src/common.hpp"

//#include "../minimal_substrings/naive_minimal_substrings.hpp"

using namespace std;
using namespace stool;

//using INDEXTYPE = int64_t;
//using CHAR = char;






int main(int argc, char *argv[])
{

    cmdline::parser p;
    p.add<string>("input_file", 'i', "input file name", true);
    p.add<std::string>("msubstr_file", 'm', "(option) The file name of minimal substrings for the input text file(the default minimal substrings filename is 'input_file.msub')", false, "");
    p.add<uint64_t>("k-attr", 'k', "(option) the value of k-attractor", false, 0);
    p.add<string>("attractor_file", 'a', "Attractors file name", true);
    p.add<string>("attractor_file_type", 't', "(option) Input attractor file type(binary or text)", true);

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    std::string mSubstrFile = p.get<std::string>("msubstr_file");
    uint64_t k_attr = p.get<uint64_t>("k-attr");
    string attractorFile = p.get<string>("attractor_file");
    string attractorFileType = p.get<string>("attractor_file_type");

    std::vector<uint8_t> T = stool::load_text_from_file(inputFile, true); // input text
    std::vector<uint64_t> sa = stool::construct_suffix_array(T);

    // Loading Attractor File
    vector<uint64_t> attractors;
    stool::lazy::loadAttractorFile(attractorFile, attractorFileType, attractors);

    // Loading Minimal Substrings
    std::vector<stool::LCPInterval<uint64_t>> minimalSubstrings = stool::lazy::loadOrConstructMS(mSubstrFile, T, sa, k_attr);

    stool::lazy::print_info(T, sa, minimalSubstrings, attractors);
    //auto newMS = filter(minimalSubstrings, T, sa, attractors);

    return 0;
}