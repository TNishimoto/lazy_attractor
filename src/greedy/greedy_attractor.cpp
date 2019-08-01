#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <limits>
#include <algorithm>
#include <queue>
#include "stool/src/print.hpp"

//#include <unordered_set>
#include "greedy_attractor.hpp"
using namespace std;

namespace stool
{
namespace lazy
{

std::vector<uint64_t> GreedyAttractorAlgorithm::computeFrequencyVector(std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals)
{
    std::vector<uint64_t> r;
    r.resize(sa.size(), 0);

    stool::Counter counter;
    std::cout << "Computing Position Weights..." << std::flush;
    for (LCPInterval<uint64_t> &interval : intervals)
    {
        if (interval.lcp == 0)
            continue;
        std::vector<std::pair<uint64_t, uint64_t>> coveredPositions = getSortedCoveredPositions(sa, interval);
        for (std::pair<uint64_t, uint64_t> &it : coveredPositions)
        {
            for (uint64_t x = it.first; x <= it.second; x++)
            {
                counter.increment();

                ++r[x];
            }
        }
    }
    std::cout << "[END]"<< std::endl;
    return r;
}

/*
std::vector<uint64_t> GreedyAttractorAlgorithm::computeFrequencyVector(std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals)
{
    std::vector<uint64_t> r;
    r.resize(sa.size(), 0);

    uint64_t counter = 10000;
    std::cout << "Computing Position Weights..." << std::flush;
    vector<uint64_t> coveredPositionStartVec;
    vector<uint64_t> coveredPositionEndVec;

    for (uint64_t i = 0; i < intervals.size(); i++)
    {
        LCPInterval<uint64_t> &interval = intervals[i];
        if (interval.lcp == 0)
            continue;
        std::vector<std::pair<uint64_t, uint64_t>> coveredPositions = getSortedCoveredPositions(sa, interval);

        if (--counter == 0)
        {
            std::cout << "+" << std::flush;
            counter = 10000;
        }
        for (std::pair<uint64_t, uint64_t> &it : coveredPositions)
        {
            coveredPositionStartVec.push_back(it.first);
            coveredPositionEndVec.push_back(it.second);
        }
        if (coveredPositionStartVec.size() > sa.size() || i == intervals.size() - 1)
        {
            std::cout << "[" << std::flush;
            std::sort(coveredPositionStartVec.begin(), coveredPositionStartVec.end());
            std::sort(coveredPositionEndVec.begin(), coveredPositionEndVec.end());

            uint64_t freq = 0;
            int64_t pos = sa.size() - 1;
            while (pos >= 0)
            {
                bool b = false;
                if (coveredPositionEndVec.size() > 0 && coveredPositionEndVec[coveredPositionEndVec.size() - 1] == pos)
                {
                    ++freq;
                    coveredPositionEndVec.pop_back();
                    b = true;
                }
                if (coveredPositionStartVec.size() > 0 && coveredPositionStartVec[coveredPositionStartVec.size() - 1] > pos)
                {
                    --freq;
                    coveredPositionStartVec.pop_back();
                    b = true;
                }
                if (!b)
                {
                    r[pos] += freq;
                    --pos;
                }
            }
            coveredPositionStartVec.clear();
            coveredPositionEndVec.clear();
            std::cout << "]" << std::flush;
        }
    }
    std::cout << "[END]" << std::endl;
    return r;
}
*/

std::vector<std::pair<uint64_t, uint64_t>> GreedyAttractorAlgorithm::getSortedCoveredPositions(std::vector<uint64_t> &sa, LCPInterval<uint64_t> &interval)
{
    std::vector<std::pair<uint64_t, uint64_t>> r;

    std::vector<uint64_t> posArr;
    for (uint64_t i = interval.i; i <= interval.j; i++)
    {
        posArr.push_back(sa[i]);
    }
    std::sort(posArr.begin(), posArr.end());

    for (uint64_t x = 0; x < posArr.size(); x++)
    {
        uint64_t pos = posArr[x];
        uint64_t nextPos = x + 1 < posArr.size() ? posArr[x + 1] : UINT64_MAX;
        uint64_t endPos = pos + interval.lcp - 1 < nextPos ? pos + interval.lcp - 1 : nextPos - 1;
        r.push_back(std::pair<uint64_t, uint64_t>(pos, endPos));
    }
    return r;
}
std::vector<uint64_t> GreedyAttractorAlgorithm::removeCapturedIntervals(uint64_t attractor, std::unordered_set<uint64_t> &currentIntervals, std::vector<LCPInterval<uint64_t>> &intervals, std::vector<uint64_t> &sa)
{
    std::vector<uint64_t> r;
    for (auto &it : currentIntervals)
    {
        LCPInterval<uint64_t> &interval = intervals[it];
        for (uint64_t x = interval.i; x <= interval.j; x++)
        {
            if (sa[x] <= attractor && attractor <= sa[x] + interval.lcp - 1)
            {
                r.push_back(it);
                break;
            }
        }
    }
    for (auto &it : r)
    {
        currentIntervals.erase(it);
    }
    return r;
}
void GreedyAttractorAlgorithm::decrementFrequencies(LCPInterval<uint64_t> &removedInterval, std::unordered_map<uint64_t, uint64_t> &currentFrequencies, std::vector<uint64_t> &sa)
{
    std::unordered_set<uint64_t> decrementedPositions;
    for (uint64_t x = removedInterval.i; x <= removedInterval.j; x++)
    {
        for (uint64_t y = 0; y < removedInterval.lcp; y++)
        {
            uint64_t p = sa[x] + y;
            if (decrementedPositions.find(p) == decrementedPositions.end())
            {
                uint64_t w = currentFrequencies[p];
                if (w != 1)
                {
                    currentFrequencies[p] = w - 1;
                }
                else
                {
                    currentFrequencies.erase(p);
                }

                decrementedPositions.insert(p);
            }
        }
    }
}
std::vector<uint64_t> GreedyAttractorAlgorithm::computeGreedyAttractors(vector<uint64_t> &sa, vector<LCPInterval<uint64_t>> &intervals)
{
    std::vector<uint64_t> outputAttrs;

    std::vector<uint64_t> frequencyVec = computeFrequencyVector(sa, intervals);
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

        if (counter++ % 100 == 0)
        {
            std::cout << "\r"
                      << "Computing Greedy Attractors : [" << currentIntervals.size() << "/" << intervals.size() << "]" << std::flush;
        }

        uint64_t nextAttr = UINT64_MAX;
        uint64_t maxWeight = 0;

        for (auto &it : currentFrequencies)
        {
            //std::cout << "[" << it.first << "/" << it.second << "]";
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
        //std::cout << std::endl;
        outputAttrs.push_back(nextAttr);

        std::vector<uint64_t> capturedIntervals = removeCapturedIntervals(nextAttr, currentIntervals, intervals, sa);
        //std::cout << nextAttr << ", Remove: ";
        for (auto &it : capturedIntervals)
        {
            //std::cout << intervals[it].to_string() << ", ";
            LCPInterval<uint64_t> &interval = intervals[it];
            decrementFrequencies(interval, currentFrequencies, sa);
        }
        //std::cout << std::endl;
    }
    std::cout << std::endl;

    std::sort(outputAttrs.begin(), outputAttrs.end());
    return outputAttrs;
}

} // namespace lazy
} // namespace stool