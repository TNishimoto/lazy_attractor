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
template <typename INDEX>
std::vector<std::string> toTateLines(std::vector<INDEX> &integers)
{
    std::vector<uint64_t> tmp;
    for (auto &it : integers)
        tmp.push_back(std::to_string(it).size());

    uint64_t max = *std::max_element(tmp.begin(), tmp.end());

    std::vector<std::string> r;
    r.resize(max);
    for (uint64_t i = 0; i < max; i++)
    {
        uint64_t rank = (max - i) - 1;
        r[i].resize(integers.size(), ' ');
        for (uint64_t x = 0; x < integers.size(); x++)
        {
            std::string s = std::to_string(integers[x]);
            if (rank < s.size())
                r[i][x] = s[i - (max - s.size())];
        }
    }
    return r;
}
template <typename INDEX>
uint64_t printFrequencyVector(std::vector<INDEX> &sa, std::vector<stool::LCPInterval<INDEX>> &intervals)
{
    std::vector<uint64_t> frequencyVector = stool::lazy::PositionFrequencySet::computeFrequencyVector(sa, intervals);
    std::vector<std::string> frequencyVecLines = toTateLines(frequencyVector);
    auto it = std::max_element(frequencyVector.begin(), frequencyVector.end());
    uint64_t itPos = std::distance(frequencyVector.begin(), it);

    for (auto &line : frequencyVecLines)
    {
        for (uint64_t i = 0; i < line.size(); i++)
        {
            if (i == itPos)
            {
                std::cout << "\033[31m";
                std::cout << line[i];
                std::cout << "\033[39m";
            }
            else
            {
                std::cout << line[i];
            }
        }
        std::cout << std::endl;
        //std::cout << it << std::endl;
    }

    return itPos;
}

template <typename CHAR, typename INDEX>
void print_info(std::vector<CHAR> &text, std::vector<INDEX> &sa, std::vector<stool::LCPInterval<INDEX>> &intervals, std::vector<INDEX> &attrs)
{

    std::vector<stool::LCPInterval<INDEX>> newMS = filter(intervals, text, sa, attrs);
    std::string attrLine;
    attrLine.resize(text.size(), ' ');
    for (auto &it : attrs)
    {
        attrLine[it] = '*';
    }
    std::cout << attrLine << std::endl;

    uint64_t fr = printFrequencyVector(sa, intervals);
    //std::vector<uint64_t> frequencyVec = stool::lazy::PositionFrequencySet::computeFrequencyVector(sa, newMS);
    //std::vector<std::string> frequencyVecLines = toTateLines(frequencyVec);
    //for(auto& it : frequencyVecLines) std::cout << it << std::endl;
    //stool::Printer::print(frequencyVec);

    stool::esaxx::printText<uint8_t>(text);
    stool::esaxx::printColor<uint8_t, uint64_t>(newMS, text, sa);
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

    print_info(T, sa, minimalSubstrings, attractors);
    //auto newMS = filter(minimalSubstrings, T, sa, attractors);

    return 0;
}