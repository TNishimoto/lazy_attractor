#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <unordered_set>
#include "stool/src/io.hpp"
#include "stool/src/sa_bwt_lcp.hpp"
#include <set>
#include <chrono>

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

    
    
    static uint64_t computeMaximalCoveredPosition(std::vector<uint64_t> &positionWeights, std::vector<uint64_t> &nextVec, uint64_t startPosition)
    {
        uint64_t max = 0;
        uint64_t maxPos = UINT64_MAX;
        uint64_t i = startPosition;
        while (i != UINT64_MAX)
        {
            if (positionWeights[i] > max)
            {
                max = positionWeights[i];
                maxPos = i;
            }
            i = nextVec[i];
        }
        return maxPos;
    }
    static std::pair<uint64_t, uint64_t> updateNextVecAndGetMostWeightedPosition(std::vector<uint64_t> &positionWeights, std::vector<uint64_t> &nextVec, uint64_t startPosition)
    {
        uint64_t max = 0;
        uint64_t maxPos = UINT64_MAX;
        uint64_t i = startPosition;
        uint64_t previousNonZeroPosition = UINT64_MAX;
        startPosition = UINT64_MAX;
        while (i != UINT64_MAX)
        {
            uint64_t weight = positionWeights[i];
            uint64_t next_i = nextVec[i];
            if (weight == 0)
            {
                nextVec[i] = UINT64_MAX - 1;
            }
            else
            {
                if (previousNonZeroPosition != UINT64_MAX)
                {
                    if (nextVec[previousNonZeroPosition] != i)
                        nextVec[previousNonZeroPosition] = i;
                }
                else
                {
                    startPosition = i;
                }
                previousNonZeroPosition = i;
            }

            if (weight > max)
            {
                max = weight;
                maxPos = i;
            }
            assert(i < next_i);
            i = next_i;

            //std::cout << ", i: " << i << std::flush;
        }
        //std::cout  << std::endl;

        return std::pair<uint64_t, uint64_t>(startPosition, maxPos);
    }

public:
    GreedyAttractorAlgorithm(std::vector<LCPInterval<uint64_t>> &intervals, std::vector<uint64_t> *_sa, uint64_t _blockSize);

    static void computeGreedyAttractors(std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals, uint64_t _blockSize, std::vector<uint64_t> &outputAttrs);
    static std::vector<uint64_t> computePositionWeights(std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals);
    static std::vector<std::pair<uint64_t, uint64_t>> getSortedCoveredPositions(std::vector<uint64_t> &sa, LCPInterval<uint64_t> &interval);

    
};
} // namespace lazy
} // namespace stool