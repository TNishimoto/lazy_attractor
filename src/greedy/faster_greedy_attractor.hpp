#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <unordered_set>
#include <set>
#include <chrono>
#include "greedy_attractor.hpp"
#include "g_dynamic_interval_tree.hpp"

namespace stool
{
namespace lazy
{
class FasterGreedyAttractor
{
    static std::vector<uint64_t> getCoveringIntervals(uint64_t pos, std::unordered_set<uint64_t> &currentIntervals, std::vector<LCPInterval<uint64_t>> &info, std::vector<uint64_t> &sa)
    {
        std::vector<uint64_t> r;
        for (auto &it : currentIntervals)
        {
            LCPInterval<uint64_t> &interval = info[it];
            if (interval.containsPosition(sa, pos))
            {
                r.push_back(it);
            }
        }
        return r;
    }
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
                    if (count > 1)
                    {
                        freqRankMap[count - 1].insert(y);
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

public:
    static void computeGreedyAttractors2(std::vector<uint64_t> &sa, std::vector<uint64_t> &isa, std::vector<LCPInterval<uint64_t>> &intervals, std::vector<uint64_t> &outputAttrs)
    {
        std::vector<uint64_t> parents = stool::esaxx::MinimalSubstringIterator<uint8_t, uint64_t, std::vector<uint64_t>>::constructMSIntervalParents(intervals);
        GDynamicIntervalTree g(sa, isa, intervals, parents, sa.size());

        //Initialize
        std::cout << "Initialize Greedy Data Structures" << std::endl;
        std::unordered_set<uint64_t> currentIntervals;
        for (uint64_t i=0;i<intervals.size();i++)
        {            
            if (intervals[i].lcp != 0)
                currentIntervals.insert(i);
        }
        std::vector<uint64_t> positionWeights = GreedyAttractorAlgorithm::computePositionWeights(sa, intervals);
        uint64_t maxFreq = *std::max_element(positionWeights.begin(), positionWeights.end());
        ;
        std::unordered_map<uint64_t, std::set<uint64_t>> freqRankMap;
        for (uint64_t i = 0; i <= maxFreq; i++)
        {
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

        double elapsed1, elapsed2;

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
                std::cout << remainingPositionCount << ", " << currentIntervals.size() << ", " << removedFrequencySum << ")"
                          << "[" << elapsed1 << "/" << elapsed2 << "]" << std::flush;
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
                //g.debug(currentIntervals, intervals, sa.size());

                uint64_t maximalCoveredPos = *(maxFreqSet.begin());
                outputAttrs.push_back(maximalCoveredPos);

                auto start1 = std::chrono::system_clock::now();
                //std::vector<uint64_t> coveringIntervals = getCoveringIntervals(maximalCoveredPos, currentIntervals,intervals, sa);
                std::vector<uint64_t> coveringIntervals = g.getLCPIntervals(isa[maximalCoveredPos]);
                auto end1 = std::chrono::system_clock::now();
                elapsed1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();

                /*
                std::vector<LCPInterval<uint64_t>> xx2;
                for (uint64_t i = 0; i < xx1.size(); i++)
                {
                    xx2.push_back(intervals[xx1[i]]);
                }

                //std::sort(coveringIntervals.begin(), coveringIntervals.end(), stool::esaxx::LCPIntervalComp<uint64_t>());

                std::sort(coveringIntervals.begin(), coveringIntervals.end(), [&](uint64_t xi, uint64_t yi) {
                    LCPInterval<uint64_t> &x = intervals[xi];
                    LCPInterval<uint64_t> &y = intervals[yi];

                    if (x.i == y.i)
                    {
                        if (x.j == y.j)
                        {
                            return x.lcp < y.lcp;
                        }
                        else
                        {
                            return x.j > y.j;
                        }
                    }
                    else
                    {
                        return x.i < y.i;
                    }
                });

                std::sort(xx2.begin(), xx2.end(), stool::esaxx::LCPIntervalComp<uint64_t>());
                */

                //std::cout << xx2.size() << "/" << coveringIntervals.size() << std::endl;

                //std::cout << std::endl;
                /* 
                for (auto &it : coveringIntervals)
                {
                    std::cout << it.to_string();
                }
                std::cout << std::endl;
                for (auto &it : xx2)
                {
                    std::cout << it.to_string();
                }
                std::cout << std::endl;
                */

                //assert(xx2.size() == coveringIntervals.size());

                //std::cout << "counter: " << counter << std::endl;
                /*
                if (counter > 100)
                    throw - 1;
                */

                removedFrequencySum = 0;

                auto start2 = std::chrono::system_clock::now();
                for (auto &intervalID : coveringIntervals)
                {
                    LCPInterval<uint64_t> interval = intervals[intervalID];
                    std::pair<uint64_t, uint64_t> info = decrementCounts(interval, positionWeights, freqRankMap, sa);
                    remainingPositionCount -= info.first;
                    removedFrequencySum += info.second;
                    currentIntervals.erase(intervalID);
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
                counter++;
            }
        }
        std::cout << std::endl;
        std::sort(outputAttrs.begin(), outputAttrs.end());
    }
};
} // namespace lazy
} // namespace stool