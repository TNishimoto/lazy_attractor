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
uint64_t FasterGreedyAttractor::getSpeedParameter(uint64_t element_size)
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
            if(x % 2 == 0){
                pset.erase(i);
            }else{
                pset.insert(i);
            }
        }
    }

    auto end2 = std::chrono::system_clock::now();
    double elapsed2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count();

    uint64_t ratio = (elapsed2 + elapsed1) / elapsed1;
    return ratio;
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
std::pair<uint64_t, uint64_t> FasterGreedyAttractor::decrementFrequencies(LCPInterval<uint64_t> &removedInterval, std::vector<uint64_t> &freqVec, std::vector<uint64_t> &sa)
{
    std::vector<std::pair<uint64_t, uint64_t>> coveredPositions = GreedyAttractorAlgorithm::getSortedCoveredPositions(sa, removedInterval);
    uint64_t newZeroPositionsCount = 0;
    uint64_t removedFrequencySum = 0;

    for (std::pair<uint64_t, uint64_t> &it : coveredPositions)
    {
        for (uint64_t y = it.first; y <= it.second; y++)
        {
            --freqVec[y];
            if (freqVec[y] == 0)
            {
                ++newZeroPositionsCount;
            }
            ++removedFrequencySum;
        }
    }
    return std::pair<uint64_t, uint64_t>(newZeroPositionsCount, removedFrequencySum);
}

std::vector<uint64_t> FasterGreedyAttractor::computeHighFrequencyGreedyAttractors(std::unordered_set<uint64_t> &currentIntervals, GDynamicIntervalTree &gtree, std::vector<uint64_t> &freqVec, uint64_t ratio,std::vector<uint64_t> &sa, std::vector<uint64_t> &isa, std::vector<LCPInterval<uint64_t>> &intervals)
{

    std::vector<uint64_t> outputAttrs;
    uint64_t currentTotalWeight = UINT64_MAX;
    stool::Counter printCounter;
    std::cout << "Computing high frequency greedy attractors" << std::flush;
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

        outputAttrs.push_back(nextAttr);

        std::vector<uint64_t> coveringIntervals = gtree.getAndRemoveCapturedLCPIntervals(isa[nextAttr]);
        currentTotalWeight = getTotalWeight(coveringIntervals, intervals);
        //std::cout << "high : "<< currentTotalWeight  << "/" << sa.size() << std::endl;
        for (auto &intervalID : coveringIntervals)
        {
            printCounter.increment();

            LCPInterval<uint64_t> interval = intervals[intervalID];
            std::pair<uint64_t, uint64_t> info = decrementFrequencies(interval, freqVec, sa);
            currentIntervals.erase(intervalID);
        }
    }
    std::cout << "[END]" << std::endl;
    return outputAttrs;
}

std::vector<uint64_t> FasterGreedyAttractor::computeGreedyAttractors(std::vector<uint64_t> &sa, std::vector<uint64_t> &isa, std::vector<LCPInterval<uint64_t>> &intervals)
{
    uint64_t ratio = FasterGreedyAttractor::getSpeedParameter(100000 );
    
    //Initialize
    std::cout << "Initializing greedy data structures" << std::endl;
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

    //uint64_t ratio = 1;

    std::vector<uint64_t> outputAttrs = FasterGreedyAttractor::computeHighFrequencyGreedyAttractors(currentIntervals, g, positionWeights, ratio, sa, isa, intervals);

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
        else if (positionWeights[i] > 0)
        {
            freqRankMap[positionWeights[i]].insert(i);
        }
    }
    std::cout << "[END]" << std::endl;

    std::cout << "Initializing greedy data structures[END]" << std::endl;

    uint64_t remainingPositionCount = sa.size();
    uint64_t removedFrequencySum = 0;
    stool::Counter counter;
    std::cout << "Computing greedy attractors" << std::flush;
    while (true)
    {

        if (maxFreqSet.size() == 0)
        {
            if (maxFreq == 0)
            {
                break;
            }
            else
            {
                --maxFreq;
                for (auto &maxIndex : freqRankMap[maxFreq])
                {
                    maxFreqSet.insert(maxIndex);
                }
                freqRankMap[maxFreq].clear();
            }
        }
        else
        {
            uint64_t maximalCoveredPos = *(maxFreqSet.begin());
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
                counter.increment();
#endif

                LCPInterval<uint64_t> interval = intervals[intervalID];
                std::pair<uint64_t, uint64_t> info = decrementFrequencies(interval, positionWeights, freqRankMap, maxFreqSet, maxFreq, sa);

                remainingPositionCount -= info.first;
                removedFrequencySum += info.second;
                currentIntervals.erase(intervalID);
                k++;
            }
        }
    }
    std::cout << "[END]" << std::endl;
    std::sort(outputAttrs.begin(), outputAttrs.end());
    return outputAttrs;
}
} // namespace lazy
} // namespace stool