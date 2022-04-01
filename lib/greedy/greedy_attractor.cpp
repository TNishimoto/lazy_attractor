#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <limits>
#include <algorithm>
#include <queue>
#include "stool/include/print.hpp"

#include "esaxx/include/minimal_substrings/minimal_substring_iterator.hpp"
#include "../include/greedy/position_frequency_set.hpp"

//#include <unordered_set>
#include "../include/greedy/greedy_attractor.hpp"
using namespace std;

namespace stool
{
namespace lazy
{


std::vector<uint64_t> GreedyAttractorAlgorithm::computeGreedyAttractors(vector<uint64_t> &sa, vector<LCPInterval<uint64_t>> &intervals)
{
    std::vector<uint64_t> outputAttrs;

    std::vector<uint64_t> frequencyVec = PositionFrequencySet::computeFrequencyVector(sa, intervals);
    std::unordered_map<uint64_t, uint64_t> currentFrequencies;
    for (uint64_t i = 0; i < frequencyVec.size(); i++)
    {
        currentFrequencies[i] = frequencyVec[i];
    }

    std::unordered_set<uint64_t> currentIntervals;
    for (uint64_t i = 0; i < intervals.size(); i++)
    {
        if (intervals[i].lcp != 0)
            currentIntervals.insert(i);
    }

    uint64_t counter = 0;

    while (currentIntervals.size() > 0)
    {

        if (counter++ % 100 == 0 && sa.size() > 10000)
        {
            std::cout << "\r"
                      << "Computing greedy attractors(Naive algorithm) : [" << currentIntervals.size() << "/" << intervals.size() << "]" << std::flush;
        }

        uint64_t nextAttr = UINT64_MAX;
        uint64_t maxWeight = 0;

        for (auto &it : currentFrequencies)
        {
            if (it.second > maxWeight)
            {
                nextAttr = it.first;
                maxWeight = it.second;
            }
            else if (it.second == maxWeight && it.first < nextAttr)
            {
                nextAttr = it.first;
            }
        }
        outputAttrs.push_back(nextAttr);

        std::vector<uint64_t> capturedIntervals = GDynamicIntervalTree::removeCapturedIntervals(nextAttr, currentIntervals, intervals, sa);
        for (auto &it : capturedIntervals)
        {
            LCPInterval<uint64_t> &interval = intervals[it];
            PositionFrequencySet::decrementFrequenciesInFreqMap(interval, currentFrequencies, sa);
        }
    }
    if(sa.size() > 10000)std::cout << std::endl;

    std::sort(outputAttrs.begin(), outputAttrs.end());
    return outputAttrs;
}

uint64_t GreedyAttractorAlgorithm::getSpeedParameter(uint64_t element_size)
{
    auto start1 = std::chrono::system_clock::now();
    std::vector<uint64_t> pvec;
    pvec.resize(element_size, 0);

    for (uint64_t x = 0; x < 100; x++)
    {
        for (uint64_t i = 0; i < element_size; i++)
        {
            ++pvec[i];
        }
    }
    auto end1 = std::chrono::system_clock::now();
    double elapsed1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
    pvec.resize(0);
    pvec.shrink_to_fit();

    auto start2 = std::chrono::system_clock::now();
    std::unordered_set<uint64_t> pset;
    for (uint64_t x = 0; x < 100; x++)
    {
        for (uint64_t i = 0; i < element_size; i++)
        {
            if (x % 2 == 0)
            {
                pset.erase(i);
            }
            else
            {
                pset.insert(i);
            }
        }
    }

    auto end2 = std::chrono::system_clock::now();
    double elapsed2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count();

    uint64_t ratio = (elapsed2 + elapsed1) / elapsed1;
    return ratio;
}

std::vector<uint64_t> GreedyAttractorAlgorithm::computeHighFrequencyGreedyAttractors(std::unordered_set<uint64_t> &currentIntervals, GDynamicIntervalTree &gtree, std::vector<uint64_t> &freqVec, uint64_t ratio, std::vector<uint64_t> &sa, std::vector<uint64_t> &isa, std::vector<LCPInterval<uint64_t>> &intervals)
{
    bool isPrint = sa.size() > 10000;
    std::vector<uint64_t> outputAttrs;
    uint64_t currentTotalWeight = sa.size() + 1;
    stool::Counter printCounter;
    if(isPrint)std::cout << "Computing high frequency greedy attractors" << std::flush;
    while (currentTotalWeight * ratio > sa.size())
    {
        uint64_t nextAttr = UINT64_MAX;
        uint64_t maxWeight = 0;

        for (uint64_t i = 0; i < freqVec.size(); i++)
        {
            if (freqVec[i] > maxWeight)
            {
                nextAttr = i;
                maxWeight = freqVec[i];
            }
        }
        if(nextAttr != UINT64_MAX){
            break;
        }
        assert(nextAttr != UINT64_MAX);

        outputAttrs.push_back(nextAttr);

        std::vector<uint64_t> coveringIntervals = gtree.getAndRemoveCapturedLCPIntervals(isa[nextAttr]);

        currentTotalWeight = getTotalWeight(coveringIntervals, intervals);

        //std::cout << "high : "<< currentTotalWeight  << "/" << sa.size() << std::endl;
        for (auto &intervalID : coveringIntervals)
        {
            if(isPrint)printCounter.increment();

            LCPInterval<uint64_t> interval = intervals[intervalID];
            std::pair<uint64_t, uint64_t> info = PositionFrequencySet::decrementFrequenciesInFreqVec(interval, freqVec, sa);

            currentIntervals.erase(intervalID);
        }

    }
    if(isPrint)std::cout << "[END]" << std::endl;
    return outputAttrs;
}

std::vector<uint64_t> GreedyAttractorAlgorithm::computeFasterGreedyAttractors(std::vector<uint64_t> &sa, std::vector<uint64_t> &isa, std::vector<LCPInterval<uint64_t>> &intervals)
{
    bool isPrint = sa.size() > 10000;
    uint64_t ratio = GreedyAttractorAlgorithm::getSpeedParameter(100000);

    //Initialize
    if(isPrint)std::cout << "Initializing greedy data structures" << std::endl;
    //std::vector<uint64_t> parents = stool::esaxx::MinimalSubstringIterator<uint8_t, uint64_t, std::vector<uint64_t>>::constructMSIntervalParents(intervals);
    std::vector<uint64_t> parents = stool::esaxx::MinimalSubstringIntervals<uint8_t, uint64_t, std::vector<uint64_t>>::iterator<>::constructMSIntervalParents(intervals);


    GDynamicIntervalTree g(sa, isa, intervals, parents, sa.size());
    g.construct();

    std::unordered_set<uint64_t> currentIntervals;
    for (uint64_t i = 0; i < intervals.size(); i++)
    {
        if (intervals[i].lcp != 0)
            currentIntervals.insert(i);
    }
    std::vector<uint64_t> positionWeights = PositionFrequencySet::computeFrequencyVector(sa, intervals);

    //uint64_t ratio = 1;

    std::vector<uint64_t> outputAttrs = GreedyAttractorAlgorithm::computeHighFrequencyGreedyAttractors(currentIntervals, g, positionWeights, ratio, sa, isa, intervals);
    PositionFrequencySet pfs;
    pfs.construct(positionWeights);

    if(isPrint)std::cout << "Initializing greedy data structures[END]" << std::endl;
    uint64_t remainingPositionCount = sa.size();
    uint64_t removedFrequencySum = 0;
    stool::Counter counter;
    if(isPrint)std::cout << "Computing greedy attractors" << std::flush;
    uint64_t maximalCoveredPos = pfs.getMostFrequentPosition();
    while (maximalCoveredPos != UINT64_MAX)
    {
        outputAttrs.push_back(maximalCoveredPos);

        std::vector<uint64_t> coveringIntervals = g.getAndRemoveCapturedLCPIntervals(isa[maximalCoveredPos]);

        removedFrequencySum = 0;
        uint64_t totalWeight = getTotalWeight(coveringIntervals, intervals);
        //std::cout << "standard : " << maxFreq << "/" << totalWeight << "/" << sa.size() << std::endl;
        uint64_t k = 0;

        for (auto &intervalID : coveringIntervals)
        {
#ifdef DEBUG_PRINT
            std::cout << "[Attrs, RemainingPositions, LCPIntervals, RemovedFrequency, coveringIntervals] = [" << outputAttrs.size() << ", " << remainingPositionCount << ", " << currentIntervals.size() << "," << removedFrequencySum << "," << coveringIntervals.size() << "," << k << "]\r" << std::flush;
#else
            if(isPrint)counter.increment();
#endif

            LCPInterval<uint64_t> interval = intervals[intervalID];
            std::pair<uint64_t, uint64_t> info = pfs.decrementFrequencies(interval, positionWeights, sa);

            remainingPositionCount -= info.first;
            removedFrequencySum += info.second;
            currentIntervals.erase(intervalID);
            k++;

            maximalCoveredPos = pfs.getMostFrequentPosition();
        }
    }
    if(isPrint)std::cout << "[END]" << std::endl;
    std::sort(outputAttrs.begin(), outputAttrs.end());
    return outputAttrs;
}

} // namespace lazy
} // namespace stool