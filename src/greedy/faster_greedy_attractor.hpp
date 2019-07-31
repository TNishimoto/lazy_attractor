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
    static std::vector<uint64_t> getCoveringIntervals(uint64_t pos, std::unordered_set<uint64_t> &currentIntervals, std::vector<LCPInterval<uint64_t>> &info, std::vector<uint64_t> &sa);
    static std::pair<uint64_t, uint64_t> decrementCounts(LCPInterval<uint64_t> &removedInterval, std::vector<uint64_t> &countVec, std::unordered_map<uint64_t, std::set<uint64_t>> &freqRankMap, std::vector<uint64_t> &sa);

public:
    static std::vector<uint64_t> computeGreedyAttractors(std::vector<uint64_t> &sa, std::vector<uint64_t> &isa, std::vector<LCPInterval<uint64_t>> &intervals);
};
} // namespace lazy
} // namespace lazy