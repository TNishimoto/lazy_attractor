#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <stack>

#include "lcp_interval.hpp"
#include "minimal_substrings.hpp"
#include "offline_suffix_tree.hpp"

namespace stool
{

class MSTStackInfo
{
  public:
    uint64_t parent;
    uint16_t child;
    uint64_t parentIntervalID;
    MSTStackInfo(uint64_t _parent, uint16_t _child, uint64_t _parentIntervalID) : parent(_parent), child(_child), parentIntervalID(_parentIntervalID)
    {
    }
};
class MinimalSubstringsTreeConstruction
{
  public:
    /*
    static void computeRangeArray(vector<LCPInterval> &intervals, vector<uint64_t> &parents, uint64_t textSize)
    {
        std::cout << "computeRangeArray" << std::endl;
        vector<std::pair<uint64_t, uint64_t>> intervalVec;
        intervalVec.resize(intervals.size());
        for (uint64_t i = 0; i < intervals.size(); i++)
        {
            intervalVec[i] = std::pair<uint64_t, uint64_t>(intervals[i].i, intervals[i].j);
        }
        parents.resize(parents.size() + textSize, UINT64_MAX);
        uint64_t intervalNum = intervals.size();
        for (uint64_t i = 0; i < intervals.size(); i++)
        {
            if (parents[i] != UINT64_MAX)
            {
                auto &interval = intervals[i];
                auto &parentInterval = intervalVec[parents[i]];
                if (parentInterval.first < interval.i)
                {
                    for (uint64_t x = parentInterval.first; x < interval.i; x++)
                    {
                        parents[intervalNum + x] = parents[i];
                    }
                }
                intervalVec[parents[i]] = std::pair<uint64_t, uint64_t>(interval.j + 1, parentInterval.second);
            }
        }
        for (uint64_t i = 0; i < intervals.size(); i++)
        {
            auto &interval = intervalVec[i];
            for (uint64_t x = interval.first; x <= interval.second; x++)
            {
                parents[intervalNum + x] = i;
            }
        }
        std::cout << "Done." << std::endl;
    }
    */
    

    static void computeMinimalSubstringsTreeWithoutIndexLeave(string &text, vector<LCPInterval> &outputIntervals, vector<uint64_t> &outputParents);

    /*
    static void compute(string &text, vector<LCPInterval> &outputIntervals, vector<uint64_t> &outputParents)
    {
        //computeMinimalSubstringsTreeWithoutIndexLeave(text, outputIntervals, outputParents);
        CSTHelper::constructMSTree(text, outputIntervals, outputParents);
        computeRangeArray(outputIntervals, outputParents, text.size());
    }
    */
    static bool isMinimalSubstring(OfflineSuffixTree &st, uint64_t id);
    public:
    static void construct(string &text, vector<LCPInterval> &outputIntervals, vector<uint64_t> &outputParents);
    /*
    static void compute2(string &text, vector<LCPInterval> &outputIntervals, vector<uint64_t> &outputParents)
    {
        construct(text, outputIntervals, outputParents);
        computeRangeArray(outputIntervals, outputParents, text.size());
    }
    */
};
} // namespace stool