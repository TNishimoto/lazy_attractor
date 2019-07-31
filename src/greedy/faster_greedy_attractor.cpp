#include "faster_greedy_attractor.hpp"
#include "esaxx/src/minimal_substrings/minimal_substring_iterator.hpp"

namespace stool
{
namespace lazy
{
std::vector<uint64_t> FasterGreedyAttractor::getCoveringIntervals(uint64_t pos, std::unordered_set<uint64_t> &currentIntervals, std::vector<LCPInterval<uint64_t>> &info, std::vector<uint64_t> &sa)
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
std::pair<uint64_t, uint64_t> FasterGreedyAttractor::decrementCounts(LCPInterval<uint64_t> &removedInterval, std::vector<uint64_t> &countVec, std::unordered_map<uint64_t, std::set<uint64_t>> &freqRankMap, std::vector<uint64_t> &sa)
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
std::vector<uint64_t> FasterGreedyAttractor::computeGreedyAttractors(std::vector<uint64_t> &sa, std::vector<uint64_t> &isa, std::vector<LCPInterval<uint64_t>> &intervals)
{
    std::vector<uint64_t> outputAttrs;

    std::vector<uint64_t> parents = stool::esaxx::MinimalSubstringIterator<uint8_t, uint64_t, std::vector<uint64_t>>::constructMSIntervalParents(intervals);
    GDynamicIntervalTree g(sa, isa, intervals, parents, sa.size());

    //Initialize
    std::cout << "Initialize Greedy Data Structures" << std::endl;
    std::unordered_set<uint64_t> currentIntervals;
    for (uint64_t i = 0; i < intervals.size(); i++)
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

    std::cout << "Initialize Greedy Data Structures[END]" << std::endl;

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
                      << "Computing Greedy Attractors : [" << currentIntervals.size() << "/" << minimalSubstringCount << "]" << std::flush;
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

            //std::vector<uint64_t> coveringIntervals = getCoveringIntervals(maximalCoveredPos, currentIntervals,intervals, sa);
            std::vector<uint64_t> coveringIntervals = g.getAndRemoveCapturedLCPIntervals(isa[maximalCoveredPos]);

            removedFrequencySum = 0;

            for (auto &intervalID : coveringIntervals)
            {
                LCPInterval<uint64_t> interval = intervals[intervalID];
                std::pair<uint64_t, uint64_t> info = decrementCounts(interval, positionWeights, freqRankMap, sa);
                remainingPositionCount -= info.first;
                removedFrequencySum += info.second;
                currentIntervals.erase(intervalID);
            }

            counter++;
        }
    }
    std::cout << std::endl;
    std::sort(outputAttrs.begin(), outputAttrs.end());
    return outputAttrs;
}
} // namespace lazy
} // namespace stool