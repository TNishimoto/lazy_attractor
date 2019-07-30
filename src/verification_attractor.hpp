#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <unordered_set>
#include "stool/src/io.hpp"
#include "stool/src/sa_bwt_lcp.hpp"
#include <sdsl/rmq_support.hpp> // include header for range minimum queries
//using namespace std;

using namespace sdsl;
namespace stool
{
namespace lazy
{
class VerificationAttractor
{
public:
    static std::vector<uint64_t> getFreeIntervals(std::vector<uint64_t> &sa, std::vector<uint64_t> &isa, std::vector<LCPInterval<uint64_t>> &intervals, std::vector<uint64_t> &attractors)
    {
        std::vector<uint64_t> outputFreeIntervalIndexes;

        uint64_t n = sa.size();
        int_vector<> v(n, INT64_MAX);

        uint64_t m = 0;
        for (uint64_t i = 0; i < n; i++)
        {
            v[isa[i]] = attractors[m] - i;
            if (i == attractors[m])
            {
                m++;
            }
            if (i % 100000 == 0)
                std::cout << "\r"
                          << "Constructing RMQ data structures... : [" << i << "/" << n << "]" << std::flush;
        }
        std::cout << std::endl;
        rmq_succinct_sada<> rmq(&v);

        for (uint64_t i = 0; i < intervals.size(); i++)
        {
            if(intervals[i].lcp == 0) continue;
            uint64_t p = rmq(intervals[i].i, intervals[i].j);
            if (v[p] + 1 > intervals[i].lcp)
            {
                outputFreeIntervalIndexes.push_back(i);
            }
            if (i % 100000 == 0)
                std::cout << "\r"
                          << "Checking attractors... : [" << i << "/" << intervals.size() << "]" << std::flush;
        }
        std::cout << std::endl;

        return outputFreeIntervalIndexes;
    }
};
} // namespace lazy
} // namespace stool