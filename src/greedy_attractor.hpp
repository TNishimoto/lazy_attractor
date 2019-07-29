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

    static std::pair<uint64_t, uint64_t> decrementCounts(LCPInterval<uint64_t> &removedInterval, std::vector<uint64_t> &countVec, std::unordered_map<uint64_t, std::set<uint64_t>> &freqRankMap, std::vector<uint64_t> &sa)
    {
        std::vector<std::pair<uint64_t, uint64_t>> coveredPositions = GreedyAttractorAlgorithm::getSortedCoveredPositions(sa, removedInterval);
        //std::cout << "a" << std::flush;
        uint64_t newZeroPositionsCount = 0;
        uint64_t removedFrequencySum = 0;

        for (std::pair<uint64_t, uint64_t> &it : coveredPositions)
        {
            for (uint64_t y = it.first; y <= it.second; y++)
            {
                if (countVec[y] != UINT64_MAX)
                {
                    uint64_t count = countVec[y];
                    freqRankMap[count].erase(y);
                    if(count > 1){
                        freqRankMap[count-1].insert(y);
                    }

                    --countVec[y];
                    if (countVec[y] == 0)
                    {
                        ++newZeroPositionsCount;
                    }
                    ++removedFrequencySum;
                }
            }
        }
        return std::pair<uint64_t, uint64_t>(newZeroPositionsCount, removedFrequencySum);

        //std::cout << "b" << std::flush;
    }
    static std::vector<LCPInterval<uint64_t>> getCoveringIntervals(uint64_t pos, std::unordered_set<LCPInterval<uint64_t>> &currentIntervals, std::vector<uint64_t> &sa)
    {
        std::vector<LCPInterval<uint64_t>> r;
        for (auto &it : currentIntervals)
        {
            if (it.containsPosition(sa, pos))
            {
                r.push_back(it);
            }
        }
        return r;
    }
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

    static void computeGreedyAttractors2(std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals, std::vector<uint64_t> &outputAttrs)
    {
        //Initialize
        std::cout << "Initialize Greedy Data Structures" << std::endl;
        std::unordered_set<LCPInterval<uint64_t>> currentIntervals;
        for (auto &it : intervals)
        {
            currentIntervals.insert(it);
        }
        std::vector<uint64_t> positionWeights = computePositionWeights(sa, intervals);
        uint64_t maxFreq = *std::max_element(positionWeights.begin(), positionWeights.end());;
        std::unordered_map<uint64_t, std::set<uint64_t>> freqRankMap;
        for(uint64_t i=0;i<= maxFreq ;i++){
            freqRankMap[i] = (std::set<uint64_t>());
        }

        for (uint64_t i = 0; i < positionWeights.size(); i++)
        {
            freqRankMap[positionWeights[i]].insert(i);
        }

        /*
        std::vector<uint64_t> nextVec;
        nextVec.resize(sa.size(), UINT64_MAX);
        for (uint64_t i = 0; i < nextVec.size() - 1; i++)
        {
            nextVec[i] = i + 1;
        }
        */
        std::cout << "Initialize Greedy Data Structures[END]" << std::endl;

        double elapsed1, elapsed2, elapsed3;

        //uint64_t startPosition = 0;
        uint64_t remainingPositionCount = sa.size();
        uint64_t removedFrequencySum = 0;
        uint64_t counter = 0;
        uint64_t minimalSubstringCount = intervals.size();
        while (true)
        {
#ifdef DEBUG_PRINT
            std::cout << "[Attrs, RemainingPositions, LCPIntervals, RemovedFrequency] = [" << outputAttrs.size() << ", " << remainingPositionCount << ", " << currentIntervals.size() << "," << removedFrequencySum << "]\r" << std::flush;
#else
            if (counter % 100 == 0)
            {
                std::cout << "\r"
                          << "Computing Greedy Attractors : [" << currentIntervals.size() << "/" << minimalSubstringCount << "], (";
                std::cout << remainingPositionCount << ", " << currentIntervals.size() << ", " << removedFrequencySum << ")" << "[" << elapsed1 << "/" << elapsed2 << "]" << std::flush;
            }
#endif
            auto maxFreqSet = freqRankMap[maxFreq];
            if (maxFreqSet.size() == 0)
            {
                if (maxFreq == 0)
                {
                    break;
                }
                else
                {
                    --maxFreq;
                }
            }
            else
            {
                uint64_t maximalCoveredPos = *(maxFreqSet.begin());
                    outputAttrs.push_back(maximalCoveredPos);

                auto start1 = std::chrono::system_clock::now();
                std::vector<LCPInterval<uint64_t>> coveringIntervals = getCoveringIntervals(maximalCoveredPos, currentIntervals, sa);
                auto end1 = std::chrono::system_clock::now();
                elapsed1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();

                removedFrequencySum = 0;

                auto start2 = std::chrono::system_clock::now();
                for (auto &coveringInterval : coveringIntervals)
                {
                    std::pair<uint64_t, uint64_t> info = decrementCounts(coveringInterval, positionWeights, freqRankMap, sa);
                    remainingPositionCount -= info.first;
                    removedFrequencySum += info.second;
                    currentIntervals.erase(coveringInterval);
                }
                auto end2 = std::chrono::system_clock::now();
                elapsed2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count();
                
                /*
                //std::cout << "+" << std::flush;
                std::pair<uint64_t, uint64_t> p = updateNextVecAndGetMostWeightedPosition(positionWeights, nextVec, startPosition);
                //std::cout << "-" << std::flush;

                startPosition = p.first;
                uint64_t maximalCoveredPos = p.second;
                //std::cout << "start: " << startPosition << "/" << maximalCoveredPos << std::endl;
                if (startPosition == UINT64_MAX)
                {
                    break;
                }
                else
                {
                    outputAttrs.push_back(maximalCoveredPos);
                    //std::cout << maximalCoveredPos << std::endl;
                    std::vector<LCPInterval<uint64_t>> coveringIntervals = getCoveringIntervals(maximalCoveredPos, currentIntervals, sa);
                    //std::cout << "*" << std::flush;
                    removedFrequencySum = 0;
                    for (auto &coveringInterval : coveringIntervals)
                    {
                        std::pair<uint64_t, uint64_t> info = decrementCounts(coveringInterval, positionWeights, sa);
                        remainingPositionCount -= info.first;
                        removedFrequencySum += info.second;
                        currentIntervals.erase(coveringInterval);
                    }
                    //std::cout << "/" << std::flush;
                }
                */
            }

            counter++;
        }
        std::cout << std::endl;
        std::sort(outputAttrs.begin(), outputAttrs.end());
    }
};
} // namespace lazy
} // namespace stool