#include "position_frequency_set.hpp"
#include "stool/src/print.hpp"
namespace stool
{
namespace lazy
{

    PositionFrequencySet::PositionFrequencySet()
    {
    }
    void PositionFrequencySet::construct(std::vector<uint64_t> &positionWeights)
    {
        bool isPrint = positionWeights.size() > 1000;
        maxFreq = *std::max_element(positionWeights.begin(), positionWeights.end());
        if(isPrint)std::cout << "Constructing frequency rank map..." << std::flush;
        stool::Counter counter1;
        for (uint64_t i = 0; i <= maxFreq; i++)
        {
            freqRankMap[i] = (std::unordered_set<uint64_t>());
        }

        for (uint64_t i = 0; i < positionWeights.size(); i++)
        {
            if(isPrint)counter1.increment();
            if (maxFreq == positionWeights[i])
            {
                maxFreqSet.insert(i);
            }
            else if (positionWeights[i] > 0)
            {
                freqRankMap[positionWeights[i]].insert(i);
            }
        }
        if(isPrint)std::cout << "[END]" << std::endl;
    }
    uint64_t PositionFrequencySet::getMostFrequentPosition()
    {
        while (maxFreq > 0)
        {
            if (maxFreqSet.size() == 0)
            {
                --maxFreq;
                for (auto &maxIndex : freqRankMap[maxFreq])
                {
                    maxFreqSet.insert(maxIndex);
                }
                freqRankMap[maxFreq].clear();
            }
            else
            {
                uint64_t maximalCoveredPos = *(maxFreqSet.begin());
                return maximalCoveredPos;
            }
        }
        return UINT64_MAX;
    }
    std::pair<uint64_t, uint64_t> PositionFrequencySet::decrementFrequencies(LCPInterval<uint64_t> &removedInterval, std::vector<uint64_t> &countVec, std::vector<uint64_t> &sa)
    {
        std::vector<std::pair<uint64_t, uint64_t>> coveredPositions = getSortedCoveredPositions(sa, removedInterval);
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
                        this->maxFreqSet.erase(y);
                    }
                    else
                    {
                        this->freqRankMap[count].erase(y);
                    }
                    if (count > 1)
                    {
                        this->freqRankMap[count - 1].insert(y);
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
    }
    std::pair<uint64_t, uint64_t> PositionFrequencySet::decrementFrequenciesInFreqVec(LCPInterval<uint64_t> &removedInterval, std::vector<uint64_t> &freqVec, std::vector<uint64_t> &sa)
    {
        std::vector<std::pair<uint64_t, uint64_t>> coveredPositions = getSortedCoveredPositions(sa, removedInterval);
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
    void PositionFrequencySet::decrementFrequenciesInFreqMap(LCPInterval<uint64_t> &removedInterval, std::unordered_map<uint64_t, uint64_t> &currentFrequencies, std::vector<uint64_t> &sa)
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
    std::vector<std::pair<uint64_t, uint64_t>> PositionFrequencySet::getSortedCoveredPositions(std::vector<uint64_t> &sa, LCPInterval<uint64_t> &interval)
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


    std::vector<uint64_t> PositionFrequencySet::computeFrequencyVector(std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals)
{
    bool isPrint = sa.size() > 10000;
    std::vector<uint64_t> r;
    r.resize(sa.size(), 0);

    stool::Counter counter;
    if(isPrint)std::cout << "Computing frequencies on positions..." << std::flush;
    for (LCPInterval<uint64_t> &interval : intervals)
    {
        if (interval.lcp == 0)
            continue;
        /*
        std::unordered_set<uint64_t> incrementedPositions;
        for (uint64_t x = interval.i; x <= interval.j; x++)
        {
            for (uint64_t y = 0; y < interval.lcp; y++)
            {
                uint64_t p = sa[x]+y;
                counter.increment();
                if (incrementedPositions.find(p) == incrementedPositions.end())
                {
                    r[p]++;

                    incrementedPositions.insert(p);
                }
            }
        }
        */

        std::vector<std::pair<uint64_t, uint64_t>> coveredPositions = getSortedCoveredPositions(sa, interval);
        for (std::pair<uint64_t, uint64_t> &it : coveredPositions)
        {
            for (uint64_t x = it.first; x <= it.second; x++)
            {
                if(isPrint)counter.increment();
                ++r[x];
            }
        }
    }
    if(isPrint)std::cout << "[END]" << std::endl;
    return r;
}
std::vector<uint64_t> PositionFrequencySet::computeFrequencyVector(std::unordered_set<uint64_t> &currentIntervals, std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals)
{

    std::vector<uint64_t> r;
    r.resize(sa.size(), 0);

    for (auto &intervalID : currentIntervals)
    {
        LCPInterval<uint64_t> &interval = intervals[intervalID];
        if (interval.lcp == 0)
            continue;

        std::vector<std::pair<uint64_t, uint64_t>> coveredPositions = getSortedCoveredPositions(sa, interval);
        for (std::pair<uint64_t, uint64_t> &it : coveredPositions)
        {
            for (uint64_t x = it.first; x <= it.second; x++)
            {
                ++r[x];
            }
        }
    }
    return r;
}

} // namespace lazy
} // namespace stool