#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <unordered_set>
#include "stool/include/io.hpp"
#include "stool/include/sa_bwt_lcp.hpp"
#include "g_dynamic_interval_tree.hpp"
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

    static uint64_t getTotalWeight(std::vector<uint64_t> &currentIntervals, std::vector<LCPInterval<uint64_t>> &intervals)
    {
        uint64_t w = 0;
        for (auto &it : currentIntervals)
        {
            LCPInterval<uint64_t> &interval = intervals[it];
            w += (interval.j - interval.i + 1) * interval.lcp;
        }
        return w;
    }
        static uint64_t getSpeedParameter(uint64_t element_size);

    static std::vector<uint64_t> computeHighFrequencyGreedyAttractors(std::unordered_set<uint64_t> &currentIntervals, GDynamicIntervalTree &gtree, std::vector<uint64_t> &freqVec, uint64_t ratio, std::vector<uint64_t> &sa, std::vector<uint64_t> &isa, std::vector<LCPInterval<uint64_t>> &intervals);

public:

    static std::vector<uint64_t> computeGreedyAttractors(std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals);
    static std::vector<uint64_t> computeFasterGreedyAttractors(std::vector<uint64_t> &sa, std::vector<uint64_t> &isa, std::vector<LCPInterval<uint64_t>> &intervals);


};

} // namespace lazy
} // namespace lazy