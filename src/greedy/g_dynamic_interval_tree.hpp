#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <unordered_set>
#include <set>
#include <chrono>
#include "../lazy/dynamic_interval_tree.hpp"
#include "stool/src/print.hpp"
namespace stool
{
namespace lazy
{
class GDynamicIntervalTree
{
private:
    DynamicIntervalTree tree;
    std::vector<uint64_t> distanceVec;
    // msVec[i] stores positions such that the farthest interval of the position is the i-th interval.
    //std::vector<std::vector<uint64_t>> msVec;
    std::vector<uint64_t> &sa;
    std::vector<uint64_t> &isa;
    std::vector<LCPInterval<uint64_t>> &intervals;

public:
    GDynamicIntervalTree(std::vector<uint64_t> &_sa, std::vector<uint64_t> &_isa, std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents, uint64_t textSize);
    
    void construct();

    std::vector<uint64_t> getAndRemoveCapturedLCPIntervals(uint64_t sa_index);
    void debug(std::unordered_set<uint64_t> &currentIntervals, std::vector<LCPInterval<uint64_t>> &intervals, uint64_t textSize);
    static std::vector<uint64_t> removeCapturedIntervals(uint64_t attractor, std::unordered_set<uint64_t> &currentIntervals, std::vector<LCPInterval<uint64_t>> &intervals, std::vector<uint64_t> &sa);
};
} // namespace lazy
} // namespace stool