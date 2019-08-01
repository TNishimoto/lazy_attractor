#include "faster_greedy_attractor.hpp"
#include "esaxx/src/minimal_substrings/minimal_substring_iterator.hpp"
#include "stool/src/print.hpp"
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
std::pair<uint64_t, uint64_t> FasterGreedyAttractor::decrementFrequencies(LCPInterval<uint64_t> &removedInterval, std::vector<uint64_t> &countVec, std::unordered_map<uint64_t, std::unordered_set<uint64_t>> &freqRankMap, std::set<uint64_t> &maxFreqMap, uint64_t maxFreq, std::vector<uint64_t> &sa)
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
                if (count == maxFreq)
                {
                    maxFreqMap.erase(y);
                }
                else
                {
                    freqRankMap[count].erase(y);
                }
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

    //Initialize
    std::cout << "Initialize Greedy Data Structures" << std::endl;
    std::vector<uint64_t> parents = stool::esaxx::MinimalSubstringIterator<uint8_t, uint64_t, std::vector<uint64_t>>::constructMSIntervalParents(intervals);
    GDynamicIntervalTree g(sa, isa, intervals, parents, sa.size());
    g.construct();

    std::unordered_set<uint64_t> currentIntervals;
    for (uint64_t i = 0; i < intervals.size(); i++)
    {
        if (intervals[i].lcp != 0)
            currentIntervals.insert(i);
    }
    std::vector<uint64_t> positionWeights = GreedyAttractorAlgorithm::computeFrequencyVector(sa, intervals);
    uint64_t maxFreq = *std::max_element(positionWeights.begin(), positionWeights.end());
    
    std::unordered_map<uint64_t, std::unordered_set<uint64_t>> freqRankMap;
    std::set<uint64_t> maxFreqSet;

    std::cout << "Constructing frequency rank map..." << std::flush;
    stool::Counter counter1;
    for (uint64_t i = 0; i <= maxFreq; i++)
    {
        freqRankMap[i] = (std::unordered_set<uint64_t>());
    }

    for (uint64_t i = 0; i < positionWeights.size(); i++)
    {
        counter1.increment();
        if (maxFreq == positionWeights[i])
        {
            maxFreqSet.insert(i);
        }
        else
        {
            freqRankMap[positionWeights[i]].insert(i);
        }
    }
    std::cout << "[END]"<< std::endl;

    std::cout << "Initialize Greedy Data Structures[END]" << std::endl;

    //uint64_t startPosition = 0;
    uint64_t remainingPositionCount = sa.size();
    uint64_t removedFrequencySum = 0;
    stool::Counter counter;
    //uint64_t counter = 0;
    uint64_t minimalSubstringCount = intervals.size();
    std::cout << "Computing Greedy Attractors..." << std::flush;
    while (true)
    {
#ifdef DEBUG_PRINT
        std::cout << "[Attrs, RemainingPositions, LCPIntervals, RemovedFrequency] = [" << outputAttrs.size() << ", " << remainingPositionCount << ", " << currentIntervals.size() << "," << removedFrequencySum << "]\r" << std::flush;
#endif
            //std::cout << "-" << std::flush;

        //auto maxFreqSet = freqRankMap[maxFreq];
        if (maxFreqSet.size() == 0)
        {
            if (maxFreq == 0)
            {
                break;
            }
            else
            {
                --maxFreq;
                for(auto &maxIndex : freqRankMap[maxFreq]){
                    maxFreqSet.insert(maxIndex);
                }
                freqRankMap[maxFreq].clear();
                
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
                counter.increment();
                LCPInterval<uint64_t> interval = intervals[intervalID];
                std::pair<uint64_t, uint64_t> info = decrementFrequencies(interval, positionWeights, freqRankMap,maxFreqSet, maxFreq, sa);
                remainingPositionCount -= info.first;
                removedFrequencySum += info.second;
                currentIntervals.erase(intervalID);
            }

        }
    }
    std::cout << "[END]" << std::endl;
    std::sort(outputAttrs.begin(), outputAttrs.end());
    return outputAttrs;
}
} // namespace lazy
} // namespace stool