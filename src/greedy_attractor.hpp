#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <unordered_set>
#include "stool/src/io.hpp"
#include "stool/src/sa_bwt_lcp.hpp"
//using namespace std;

namespace std
{
template <>
struct hash<stool::LCPInterval<uint64_t>>
{
    std::size_t operator()(stool::LCPInterval<uint64_t> const &key) const
    {
        return ((1234567891234ull) * key.i) ^ ((789000001234ull) * key.j) ^ key.lcp;
    }
};
} // namespace std
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
    std::unordered_set<LCPInterval<uint64_t>> currentIntervals;
    uint64_t blockSize = 1000;

    uint64_t blockNum;

    void addCount(LCPInterval<uint64_t> &interval, int64_t addValue);
    uint64_t getMaxTPosition();
    void getContainingIntervals(uint64_t pos, std::vector<LCPInterval<uint64_t>> &outputIntervals);
    void addAttractor(uint64_t pos);
    //void updateMaxPosVec();


  public:
    GreedyAttractorAlgorithm(std::vector<LCPInterval<uint64_t>> &intervals, std::vector<uint64_t> *_sa, uint64_t _blockSize);

    static void compute(std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals, uint64_t _blockSize, std::vector<uint64_t> &outputAttrs);
};
} // namespace stool