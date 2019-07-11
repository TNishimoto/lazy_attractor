#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <unordered_set>
#include "lcp_interval.hpp"
#include "sa_lcp.hpp"
//using namespace std;

namespace stool
{

class GreedyAttractorAlgorithm
{
    std::vector<bool> changedVec;
    std::vector<uint64_t> maxPosVec;

    std::vector<uint64_t> countVec;
    //vector<LCPInterval> &intervals;
    std::vector<uint64_t> &sa;
    //vector<uint64_t> isa;
    std::unordered_set<LCPInterval> currentIntervals;
    uint64_t blockSize = 1000;

    uint64_t blockNum;

    void addCount(LCPInterval &interval, int64_t addValue);
    uint64_t getMaxTPosition();
    void getContainingIntervals(uint64_t pos, std::vector<LCPInterval> &outputIntervals);
    void addAttractor(uint64_t pos);
    //void updateMaxPosVec();


  public:
    GreedyAttractorAlgorithm(std::vector<LCPInterval> &intervals, std::vector<uint64_t> *_sa, uint64_t _blockSize);

    static void compute(std::vector<uint64_t> &sa, std::vector<LCPInterval> &intervals, uint64_t _blockSize, std::vector<uint64_t> &outputAttrs);
};
} // namespace stool