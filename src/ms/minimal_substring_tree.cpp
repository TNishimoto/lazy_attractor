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
void MinimalSubstringTree::constructMSIntervalParents(vector<LCPInterval> &intervals, vector<uint64_t> &outputParents)
{
    stack<uint64_t> stack;
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

void MinimalSubstringTree::construct(string &text, vector<LCPInterval> &outputIntervals, vector<uint64_t> &outputParents)
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

    vector<uint64_t> sa, isa, lcp, parents;
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