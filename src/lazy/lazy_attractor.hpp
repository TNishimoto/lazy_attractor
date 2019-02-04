#pragma once
#include "io.h"
#include <stack>
#include "uftree.hpp"
#include "sa_lcp.hpp"
using namespace std;

namespace stool
{

class LazyAttractorAlgorithm
{

    UFTree uftree;
    stack<std::pair<uint64_t, uint64_t>> sortedMinimumSubstrings;
    //vector<bool> removedVec;
    vector<uint64_t> sa;
    vector<uint64_t> isa;
    vector<LCPInterval> intervals;
    static void checkRangeArray(vector<LCPInterval> &intervals, vector<uint64_t> &parents);
    static void constructRangeArray(vector<LCPInterval> &intervals, vector<uint64_t> &parents, uint64_t textSize);
    uint64_t getTextSize();
    uint64_t getTreeNodeID(uint64_t pos);
    uint64_t getLongestLCPIntervalID(uint64_t pos);
    bool removeLongestLCPInterval(uint64_t pos);
    bool checkRemovedInterval(uint64_t intervalID);
    void addAttractor(uint64_t pos, vector<uint64_t> &outputAttrs);
    void constructSortedMinimumSubstrings();

  public:
    LazyAttractorAlgorithm(string &text, vector<LCPInterval> &_intervals, vector<uint64_t> &_parents);
    void compute(vector<uint64_t> &outputAttrs);
};
} // namespace stool