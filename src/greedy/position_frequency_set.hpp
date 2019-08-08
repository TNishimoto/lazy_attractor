#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <unordered_set>
#include <set>
#include <chrono>
#include <unordered_map>
#include "stool/src/sa_bwt_lcp.hpp"

namespace stool
{
namespace lazy
{

class PositionFrequencySet
{
    std::unordered_map<uint64_t, std::unordered_set<uint64_t>> freqRankMap;
    //std::set<uint64_t> maxFreqMap;
    std::set<uint64_t> maxFreqSet;
    uint64_t maxFreq = 0;

public:
    PositionFrequencySet();
    void construct(std::vector<uint64_t> &positionWeights);
    uint64_t getMostFrequentPosition();
    std::pair<uint64_t, uint64_t> decrementFrequencies(LCPInterval<uint64_t> &removedInterval, std::vector<uint64_t> &countVec, std::vector<uint64_t> &sa);
    static std::pair<uint64_t, uint64_t> decrementFrequenciesInFreqVec(LCPInterval<uint64_t> &removedInterval, std::vector<uint64_t> &freqVec, std::vector<uint64_t> &sa);
    static void decrementFrequenciesInFreqMap(LCPInterval<uint64_t> &removedInterval, std::unordered_map<uint64_t, uint64_t> &currentFrequencies, std::vector<uint64_t> &sa);
    static std::vector<std::pair<uint64_t, uint64_t>> getSortedCoveredPositions(std::vector<uint64_t> &sa, LCPInterval<uint64_t> &interval);

    static std::vector<uint64_t> computeFrequencyVector(std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals);
    static std::vector<uint64_t> computeFrequencyVector(std::unordered_set<uint64_t> &currentIntervals, std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals);
};

} // namespace lazy
} // namespace stool