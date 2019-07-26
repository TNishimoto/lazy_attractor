#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <stack>

#include "lcp_interval.hpp"
//#include "minimal_substrings.hpp"
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
    

    static bool isMinimalSubstring(OfflineSuffixTree &st, uint64_t id);
    public:
    static void construct(string &text, vector<LCPInterval> &outputIntervals, vector<uint64_t> &outputParents);
};
} // namespace stool