#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <unordered_set>
#include "stool/src/io.hpp"
#include "stool/src/sa_bwt_lcp.hpp"
#include <set>
#include <chrono>

//using namespace std;

namespace std
{
template <>
struct hash<stool::LCPInterval<uint64_t>>
{
    std::size_t operator()(stool::LCPInterval<uint64_t> const &key) const
    {
        return ((1234567891234ull) * key.i) ^ ((789000001234ull) * key.j) ^ key.lcp;
    }
};
} // namespace std
namespace stool
{
namespace lazy
{
class GreedyAttractorAlgorithm
{
private:
    static std::vector<uint64_t> removeCapturedIntervals(uint64_t attractor, std::unordered_set<uint64_t> &currentIntervals, std::vector<LCPInterval<uint64_t>> &intervals, std::vector<uint64_t> &sa);
    static void decrementFrequencies(LCPInterval<uint64_t> &removedInterval, std::unordered_map<uint64_t, uint64_t> &currentFrequencies, std::vector<uint64_t> &sa);
public:

    static std::vector<uint64_t> computeGreedyAttractors(std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals);
    static std::vector<uint64_t> computeFrequencyVector(std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals);
    static std::vector<uint64_t> computeFrequencyVector(std::unordered_set<uint64_t> &currentIntervals, std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals);

    static std::vector<std::pair<uint64_t, uint64_t>> getSortedCoveredPositions(std::vector<uint64_t> &sa, LCPInterval<uint64_t> &interval);

};

} // namespace lazy
} // namespace lazy