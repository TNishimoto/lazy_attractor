#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <unordered_set>
#include "lcp_interval.hpp"
#include "sa_lcp.hpp"
using namespace std;

namespace stool
{

class GreedyAttractorAlgorithm
{
    vector<bool> changedVec;
    vector<uint64_t> maxPosVec;

    vector<uint64_t> countVec;
    //vector<LCPInterval> &intervals;
    vector<uint64_t> &sa;
    //vector<uint64_t> isa;
    unordered_set<LCPInterval> currentIntervals;
    uint64_t blockSize = 1000;

    uint64_t blockNum;

    void addCount(LCPInterval &interval, int64_t addValue);
    uint64_t getMaxTPosition();
    void getContainingIntervals(uint64_t pos, vector<LCPInterval> &outputIntervals);
    void addAttractor(uint64_t pos);
    //void updateMaxPosVec();


  public:
    GreedyAttractorAlgorithm(vector<LCPInterval> &intervals, vector<uint64_t> *_sa, uint64_t _blockSize);

    static void compute(vector<uint64_t> &sa, vector<LCPInterval> &intervals, uint64_t _blockSize, vector<uint64_t> &outputAttrs);
};
} // namespace stool