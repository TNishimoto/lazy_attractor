#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <limits>
#include <algorithm>
//#include <unordered_set>
#include "greedy_attractor.hpp"
using namespace std;

namespace stool
{
namespace lazy
{
void GreedyAttractorAlgorithm::addCount(LCPInterval<uint64_t> &interval, int64_t addValue)
{
    std::vector<std::pair<uint64_t, uint64_t>> coveredPositions = GreedyAttractorAlgorithm::getSortedCoveredPositions(this->sa, interval);
    for (std::pair<uint64_t, uint64_t> &it : coveredPositions)
    {
        for (uint64_t y = it.first; y <= it.second; y++)
        {
            if (this->countVec[y] != UINT64_MAX)
            {
                this->countVec[y] += addValue;
                uint64_t blockPos = y / this->blockSize;
                this->changedVec[blockPos] = true;
            }
        }
    }
    /*
    vector<uint64_t> posArr;
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
        for (uint64_t y = pos; y <= endPos; y++)
        {
            if (this->countVec[y] != UINT64_MAX)
            {
                this->countVec[y] += addValue;
                uint64_t blockPos = y / this->blockSize;
                this->changedVec[blockPos] = true;
            }
        }
    }
    */
}


/*
void GreedyAttractorAlgorithm::updateMaxPosVec()
{
    for (uint64_t i = 0; i < this->blockNum; i++)
    {
        if (this->changedVec[i])
        {
            uint64_t beg = i * this->blockSize;
            uint64_t max = 0;
            uint64_t maxPos = UINT64_MAX;
            for (uint64_t x = 0; x < this->blockSize; x++)
            {
                if (this->countVec[beg + x] != UINT64_MAX && max < this->countVec[beg + x])
                {
                    max = this->countVec[beg + x];
                    maxPos = beg + x;
                }
            }
            this->maxPosVec[i] = maxPos;
            this->changedVec[i] = false;
        }
    }
}
*/

/* 
    Compute the position covered by most minimal substrings.
*/
uint64_t GreedyAttractorAlgorithm::computeMaximalCoveredPosition()
{
    uint64_t p = UINT64_MAX;
    uint64_t wholeMax = 0;

    for (uint64_t i = 0; i < this->blockNum; i++)
    {

        if (this->changedVec[i])
        {
            uint64_t beg = i * this->blockSize;
            uint64_t max = 0;
            uint64_t maxPos = UINT64_MAX;
            for (uint64_t x = 0; x < this->blockSize; x++)
            {
                if (this->countVec[beg + x] != UINT64_MAX && max < this->countVec[beg + x])
                {
                    max = this->countVec[beg + x];
                    maxPos = beg + x;
                }
            }
            this->maxPosVec[i] = maxPos;
            this->changedVec[i] = false;
        }
        uint64_t mPos = this->maxPosVec[i];

        if (mPos != UINT64_MAX && this->countVec[mPos] != UINT64_MAX && wholeMax < this->countVec[mPos])
        {
            p = mPos;
            wholeMax = this->countVec[mPos];
        }
    }
    return p;
}
void GreedyAttractorAlgorithm::getContainingIntervals(uint64_t pos, vector<LCPInterval<uint64_t>> &outputIntervals)
{
    for (auto it = this->currentIntervals.begin(); it != this->currentIntervals.end(); ++it)
    {
        if (it->containsPosition(sa, pos))
        {
            outputIntervals.push_back(*it);
        }
    }
}
void GreedyAttractorAlgorithm::computeGreedyAttractors(vector<uint64_t> &sa, vector<LCPInterval<uint64_t>> &intervals, uint64_t _blockSize, vector<uint64_t> &outputAttrs)
{
    GreedyAttractorAlgorithm algo(intervals, &sa, _blockSize);

    //uint64_t prev = UINT64_MAX;
    uint64_t iter = 0;
    std::cout << "computing greedy attractors... : [" << algo.currentIntervals.size() << "]" << std::flush;

    while (algo.currentIntervals.size() > 0)
    {
        iter++;
        if (iter % 100 == 0)
            std::cout << "\r"
                      << "computing greedy attractors... : [" << algo.currentIntervals.size() << "]" << std::flush;
        uint64_t maxTPos = algo.computeMaximalCoveredPosition();
        algo.addAttractor(maxTPos);
        outputAttrs.push_back(maxTPos);
    }
    std::cout << "[END]" << std::endl;
    std::sort(outputAttrs.begin(), outputAttrs.end());
}
void GreedyAttractorAlgorithm::addAttractor(uint64_t pos)
{
    vector<LCPInterval<uint64_t>> coveringIntervals;
    this->getContainingIntervals(pos, coveringIntervals);
    for (auto &interval : coveringIntervals)
    {
        this->addCount(interval, -1);
        this->currentIntervals.erase(interval);
    }
    this->countVec[pos] = UINT64_MAX;
    //this->updateMaxPosVec();
}
GreedyAttractorAlgorithm::GreedyAttractorAlgorithm(vector<LCPInterval<uint64_t>> &intervals, vector<uint64_t> *_sa, uint64_t _blockSize) : sa(*_sa)
{
    std::cout << "Constructing Data Strutures..." << std::flush;

    this->blockSize = _blockSize;
    //constructISA(sa, isa);
    this->blockNum = (sa.size() / this->blockSize) + 1;
    this->changedVec.resize(this->blockNum, false);
    this->maxPosVec.resize(this->blockNum, UINT64_MAX);

    this->countVec.resize(this->blockNum * this->blockSize, 0);
    for (uint64_t i = 0; i < intervals.size(); i++)
    {
        addCount(intervals[i], 1);
        this->currentIntervals.insert(intervals[i]);

        if (i % 10000 == 0)
            std::cout << "\r"
                      << "Constructing Data Strutures... : [" << i << "/" << intervals.size() << "]" << std::flush;
    }
    intervals.resize(0);
    //this->updateMaxPosVec();
    std::cout << "[END]" << std::endl;

    /*
    for (auto it = this->currentIntervals.begin(); it != this->currentIntervals.end(); ++it)
    {
    }
    */

    /*
    uint64_t iter = 0;
    for (auto interval : intervals)
    {
    }
    */
}
std::vector<uint64_t> GreedyAttractorAlgorithm::computePositionWeights(std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals)
{
    std::vector<uint64_t> r;
    r.resize(sa.size(), 0);

    uint64_t counter = 0;
    for (LCPInterval<uint64_t> &interval : intervals)
    {
        if(interval.lcp == 0) continue;
        if (counter % 10000 == 0)
            std::cout << "\r"
                      << "Computing Position Weights : [" << counter << "/" << intervals.size() << "]" << std::flush;
        std::vector<std::pair<uint64_t, uint64_t>> coveredPositions = getSortedCoveredPositions(sa, interval);
        for (std::pair<uint64_t, uint64_t> &it : coveredPositions)
        {
            for (uint64_t x = it.first; x <= it.second; x++)
            {
                ++r[x];
            }
        }
        counter++;
    }
    std::cout << std::endl;
    return r;
}

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


} // namespace lazy
} // namespace stool