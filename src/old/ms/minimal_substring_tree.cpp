#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include "minimal_substring_tree.hpp"
namespace stool
{
void MinimalSubstringTree::write(std::string filepath, std::string &text)
{

    std::ofstream out(filepath, std::ios::out | std::ios::binary);
    //variables.serialize(out);
    std::hash<std::string> hash_fn;
    uint64_t hash = hash_fn(text);
    uint64_t len = text.size();
    out.write(reinterpret_cast<const char *>(&hash), sizeof(uint64_t));
    out.write(reinterpret_cast<const char *>(&len), sizeof(uint64_t));
    stool::IO::write(out, nodes, false);
    stool::IO::write(out, parents, false);

    out.close();
    //this->printInfo();
}
void MinimalSubstringTree::load(std::string filepath, std::string &text)
{
    std::ifstream m_ifs(filepath);
    bool mSubstrFileExist = m_ifs.is_open();
    if (!mSubstrFileExist)
    {
        std::cout << "\033[31m";
        std::cout << "Error: " << filepath << "does not exist." << std::endl;
        std::cout << "\033[39m" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::ifstream inputStream;
    inputStream.open(filepath, std::ios::binary);

    std::hash<std::string> hash_fn;
    uint64_t hash = hash_fn(text);
    uint64_t len = text.size();

    uint64_t load_hash;
    uint64_t load_len;

    inputStream.read((char *)&load_hash, sizeof(uint64_t));
    inputStream.read((char *)&load_len, sizeof(uint64_t));
    stool::IO::load(inputStream, nodes, false);
    stool::IO::load(inputStream, parents, false);

    inputStream.close();
    if (load_hash != hash || load_len != len)
    {
        std::cout << "\033[31m";
        std::cout << "Error: " << filepath << " is not the msub file of the input file." << std::endl;
        std::cout << "Please construct the msub file by msubstr.out.";
        std::cout << "\033[39m" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    //this->printInfo();
}
void MinimalSubstringTree::loadOrConstruct(std::string filepath, std::string *text)
{
    std::ifstream m_ifs(filepath);
    bool mSubstrFileExist = m_ifs.is_open();
    if (!mSubstrFileExist)
    {
        MinimalSubstringTree::construct(*text, this->nodes, this->parents);
        this->write(filepath, *text);
        //IO::write(filepath, this->nodes);
        //IO::write(filepath2, this->parents);
    }
    else
    {
        this->load(filepath, *text);
        //IO::load<LCPInterval>(filepath, this->nodes);
        //IO::load<uint64_t>(filepath2, this->parents);
    }
}
void MinimalSubstringTree::constructMSIntervalParents(std::vector<LCPInterval> &intervals, std::vector<uint64_t> &outputParents)
{
    std::stack<uint64_t> stack;
    outputParents.resize(intervals.size(), UINT64_MAX);
    for (uint64_t i = 0; i < intervals.size(); i++)
    {
        if (i % 100000 == 0)
            std::cout << "\r"
                      << "Computing minimal substrings... :[4/4][" << i << "/" << intervals.size() << "]" << std::flush;
        LCPInterval &interval = intervals[i];

        while (stack.size() > 0)
        {
            LCPInterval &parentInterval = intervals[stack.top()];

            if (parentInterval.i <= interval.i && interval.j <= parentInterval.j)
            {
                break;
            }
            else
            {
                stack.pop();
            }
        }
        if (stack.size() > 0)
        {
            outputParents[i] = stack.top();
        }
        stack.push(i);
    }
    std::cout << "[END]" << std::endl;
}

void MinimalSubstringTree::construct(std::string &text, std::vector<LCPInterval> &outputIntervals, std::vector<uint64_t> &outputParents)
{
    for (uint64_t i = 0; i < text.size(); i++)
    {
        if (text[i] == 0)
        {
            std::cout << "the input text contains zero." << std::endl;
            throw - 1;
        }
    }
    text.push_back(0);

    std::vector<uint64_t> sa, isa, lcp, parents;
    stool::constructSA(text, sa, isa);
    if (sa[0] != text.size() - 1)
    {
        std::cout << "the input text contains negative values." << std::endl;
        throw - 1;
    }
    constructLCP(text, lcp, sa, isa);
    isa.resize(0);
    isa.shrink_to_fit();

    std::cout << "Constructing FM-Index..." << std::endl;
    FMIndex fmi(text, sa);
    std::cout << "Computing minimal substrings..." << std::flush;
    OnlineMSInterval::construct(sa, lcp, fmi, outputIntervals);

    MinimalSubstringTree::constructMSIntervalParents(outputIntervals, outputParents);
    text.pop_back();
}

} // namespace stool