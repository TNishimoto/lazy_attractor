#pragma once
#include <stack>
#include <vector>
#include <unordered_set>
#include "stool/src/io.h"
#include "esaxx/src/minimal_substrings/minimal_substring_iterator.hpp"

namespace stool
{
namespace lazy
{

template<typename CHAR>
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
        stool::load_vector<LCPInterval<uint64_t>>(filename,r, true);
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


} // namespace lazy
} // namespace stool