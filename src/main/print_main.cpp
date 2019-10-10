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

//#include "../minimal_substrings/naive_minimal_substrings.hpp"

using namespace std;
using namespace stool;

//using INDEXTYPE = int64_t;
//using CHAR = char;

template <typename CHAR, typename INDEX>
std::vector<stool::LCPInterval<INDEX>> filter(std::vector<stool::LCPInterval<INDEX>> &intervals, std::vector<CHAR> &text, std::vector<INDEX> &sa, std::vector<INDEX> &attrs)
{
    std::vector<stool::LCPInterval<INDEX>> new_intervals;
    for (uint64_t i = 0; i < intervals.size(); i++)
    {
        stool::LCPInterval<INDEX> &interval = intervals[i];
        bool b = false;

        for (uint64_t x = interval.i; x <= interval.j; x++)
        {
            uint64_t pos = sa[x];
            for (auto &attr : attrs)
            {
                if (pos <= attr && attr < pos + interval.lcp)
                {
                    b = true;
                }
            }
        }
        if (!b)
        {
            new_intervals.push_back(interval);
        }
    }
    return new_intervals;
}

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

    std::string attrLine;
    attrLine.resize(T.size(), ' ');
    for (auto &it : attractors)
    {
        attrLine[it] = '*';
    }
    std::cout << attrLine << std::endl;

    auto newMS = filter(minimalSubstrings, T, sa, attractors);

    //stool::esaxx::print<uint8_t, uint64_t>(minimalSubstrings , T, sa);
    //stool::esaxx::printColor<uint8_t, uint64_t>(minimalSubstrings, T, sa);
    stool::esaxx::printColor<uint8_t, uint64_t>(newMS, T, sa);

    return 0;
}