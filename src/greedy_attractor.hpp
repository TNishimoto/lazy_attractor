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
namespace lazy
{
class GreedyAttractorAlgorithm
{
    std::vector<bool> changedVec;
    std::vector<uint64_t> maxPosVec;

    std::vector<uint64_t> countVec;
    std::vector<uint64_t> &sa;
    std::unordered_set<LCPInterval<uint64_t>> currentIntervals;
    uint64_t blockSize = 1000;

    uint64_t blockNum;

    void addCount(LCPInterval<uint64_t> &interval, int64_t addValue);

    /* 
    Compute the position covered by most minimal substrings.
    */
    uint64_t computeMaximalCoveredPosition();
    void getContainingIntervals(uint64_t pos, std::vector<LCPInterval<uint64_t>> &outputIntervals);
    void addAttractor(uint64_t pos);
    //void updateMaxPosVec();

public:
    GreedyAttractorAlgorithm(std::vector<LCPInterval<uint64_t>> &intervals, std::vector<uint64_t> *_sa, uint64_t _blockSize);

    static void computeGreedyAttractors(std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals, uint64_t _blockSize, std::vector<uint64_t> &outputAttrs);
    static std::vector<uint64_t> computePositionWeights(std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals);
    static std::vector<std::pair<uint64_t,uint64_t>> getSortedCoveredPositions(std::vector<uint64_t> &sa, LCPInterval<uint64_t> &interval);
    

};
} // namespace lazy
} // namespace stool