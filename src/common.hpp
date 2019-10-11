#pragma once
#include <stack>
#include <vector>
#include <unordered_set>
#include "stool/src/io.h"
#include "esaxx/src/minimal_substrings/minimal_substring_iterator.hpp"
#include "greedy/position_frequency_set.hpp"
#include "lazy/lazy_attractor.hpp"
#include "esaxx/src/main/common.hpp"

namespace stool
{
namespace lazy
{

template <typename CHAR>
std::vector<stool::LCPInterval<uint64_t>> loadOrConstructMS(std::string filename, std::vector<CHAR> &text, std::vector<uint64_t> &sa, uint64_t k_attr)
{
    std::vector<stool::LCPInterval<uint64_t>> r;
    if (filename.size() == 0)
    {
        std::vector<uint64_t> lcpArray = stool::constructLCP<CHAR, uint64_t>(text, sa);
        std::vector<CHAR> bwt = stool::constructBWT<CHAR, uint64_t>(text, sa);

        std::vector<stool::LCPInterval<uint64_t>> tmp = stool::esaxx::MinimalSubstringIterator<CHAR, uint64_t, std::vector<uint64_t>>::constructSortedMinimalSubstrings(bwt, sa, lcpArray);
        r.swap(tmp);
    }
    else
    {
        stool::load_vector<LCPInterval<uint64_t>>(filename, r, true);
    }

    if (k_attr != 0)
    {
        stool::esaxx::MinimalSubstringIterator<CHAR, uint64_t, std::vector<uint64_t>>::getKMinimalSubstrings(r, k_attr);
    }
    return r;
}

bool checkAttractorTextFile(std::string &text)
{
    std::unordered_set<char> okSet;
    okSet.insert('0');
    okSet.insert('1');
    okSet.insert('2');
    okSet.insert('3');
    okSet.insert('4');
    okSet.insert('5');
    okSet.insert('6');
    okSet.insert('7');
    okSet.insert('8');
    okSet.insert('9');
    okSet.insert(' ');
    okSet.insert(',');
    okSet.insert('\n');
    okSet.insert('\r');
    for (auto &it : text)
    {
        if (okSet.find(it) == okSet.end())
        {
            return false;
        }
    }
    return true;
}
std::string substr(std::vector<uint8_t> &text, uint64_t i, uint64_t len)
{
    std::string str;
    str.resize(len);
    for (uint64_t x = 0; x < len; x++)
    {
        if (x + i >= text.size())
        {
            throw - 1;
        }
        str[x] = (char)text[x + i];
    }
    return str;
}

template <typename List>
void split(const std::string &s, const std::string &delim, List &result)
{
    result.clear();

    using string = std::string;
    string::size_type pos = 0;

    while (pos != string::npos)
    {
        string::size_type p = s.find(delim, pos);

        if (p == string::npos)
        {
            result.push_back(s.substr(pos));
            break;
        }
        else
        {
            result.push_back(s.substr(pos, p - pos));
        }

        pos = p + delim.size();
    }
}
void loadAttractorFile(std::string attractorFile, std::string type, std::vector<uint64_t> &attractors)
{
    std::ifstream a_ifs(attractorFile);
    bool attractorFileExist = a_ifs.is_open();
    if (!attractorFileExist)
    {
        std::cout << attractorFile << " cannot open." << std::endl;
        throw - 1;
    }

    if (type == "text")
    {
        std::string text;
        IO::load(attractorFile, text);
        bool b = checkAttractorTextFile(text);
        if (!b)
        {
            std::cout << "\033[31m";
            std::cout << "Error: the text file for attractors allow to contain 0-9 characters, spaces, and commas.";
            std::cout << "\033[39m" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        std::vector<std::string> strs;
        split(text, ",", strs);
        for (auto &character : strs)
        {
            uint64_t c = atoi(character.c_str());
            attractors.push_back(c);
        }
    }
    else
    {
        stool::load_vector(attractorFile, attractors, true);
    }
    sort(attractors.begin(), attractors.end());
}

template <typename INDEX>
std::vector<stool::LCPInterval<INDEX>> filter(std::vector<stool::LCPInterval<INDEX>> &intervals, std::vector<INDEX> &sa, std::vector<INDEX> &attrs)
{
    std::vector<stool::LCPInterval<INDEX>> new_intervals;
    for (uint64_t i = 0; i < intervals.size(); i++)
    {
        stool::LCPInterval<INDEX> &interval = intervals[i];
        if (interval.lcp == 0)
            continue;

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
void printTateLines(std::vector<INDEX> &integers, uint64_t colorPos, std::string lineName)
{
    std::vector<std::string> lines = toTateLines(integers);
    for (uint64_t x = 0; x < lines.size(); x++)
    {
        std::string &line = lines[x];
        for (uint64_t i = 0; i < line.size(); i++)
        {
            if (i == colorPos)
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

        if (x == lines.size() - 1)
            std::cout << " " << lineName << "(Vertical writing)";
        std::cout << std::endl;
        //std::cout << it << std::endl;
    }
}

template <typename INDEX>
uint64_t printFrequencyVector(std::vector<INDEX> &sa, std::vector<stool::LCPInterval<INDEX>> &intervals)
{
    if(intervals.size() == 0) return UINT64_MAX;
    std::vector<uint64_t> frequencyVector = stool::lazy::PositionFrequencySet::computeFrequencyVector(sa, intervals);
    auto it = std::max_element(frequencyVector.begin(), frequencyVector.end());
    uint64_t itPos = std::distance(frequencyVector.begin(), it);
    printTateLines(frequencyVector, itPos, "Frequencies");

    return itPos;
}

template <typename CHAR, typename INDEX>
uint64_t print_info(std::vector<CHAR> &text, std::vector<INDEX> &sa, std::vector<stool::LCPInterval<INDEX>> &intervals, std::vector<INDEX> &attrs, std::string algorithm_type = "lazy")
{
    uint64_t nextAttractor = UINT64_MAX;
    //std::vector<stool::LCPInterval<INDEX>> newMS = filter(intervals, text, sa, attrs);
    uint64_t nextLazyAttr = stool::lazy::LazyAttractor::naiveComputeNextLazyAttractor(sa, intervals);
    std::vector<uint64_t> posVec;
    for (uint64_t i = 0; i < text.size(); i++)
        posVec.push_back(i);

    if(algorithm_type == "lazy" || algorithm_type == "none"){
        printTateLines(posVec, nextLazyAttr, "Positions");
        nextAttractor = nextLazyAttr;
    }else{
        printTateLines(posVec, UINT64_MAX, "Positions");
    }


    std::string attrLine;
    attrLine.resize(text.size(), ' ');

    for (auto &it : attrs)
    {
        attrLine[it] = '*';
    }
    std::cout << attrLine << " Attractors" << std::endl;
    stool::esaxx::printText<uint8_t>(text);
    if (algorithm_type == "greedy" || algorithm_type == "none")
    {
        uint64_t fr = printFrequencyVector(sa, intervals);
        nextAttractor = fr;
    }

    stool::esaxx::printColor<uint8_t, uint64_t>(intervals, text, sa, (algorithm_type != "greedy"), (algorithm_type != "greedy" ? -1 : nextAttractor), "Minimal substrings" );

    return nextAttractor;
}

} // namespace lazy
} // namespace stool