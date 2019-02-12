#include "io.h"
#include <stack>
#include "sa_lcp.hpp"
//#include "mstree.hpp"
#include "print.hpp"

#include "lazy_attractor.hpp"
using namespace std;
using namespace sdsl;

namespace stool
{
/*
void LazyAttractorAlgorithm::constructSortedMinimumSubstrings(vector<LCPInterval> &intervals, vector<uint64_t> &sa, stack<std::pair<uint64_t, uint64_t>> &outputSortedMinimumSubstrings)
{

    std::cout << "\r"
              << "Sorting minimal substrings...[1/6]" << std::flush;
    vector<std::pair<uint64_t, uint64_t>> vec;
    vec.resize(intervals.size());

    uint64_t n = sa.size();
    int_vector<> v(n, INT64_MAX);
    std::cout << "\r"
              << "Sorting minimal substrings...[2/6]" << std::flush;

    uint64_t m = 0;
    for (uint64_t i = 0; i < n; i++)
    {
        v[i] = sa[i];
    }
    std::cout << "\r"
              << "Sorting minimal substrings...[3/6]" << std::flush;
    rmq_succinct_sada<> rmq(&v);

    std::cout << "\r"
              << "Sorting minimal substrings...[4/6]" << std::flush;
    for (uint64_t i = 0; i < intervals.size(); i++)
    {
        uint64_t minPos = rmq(intervals[i].i, intervals[i].j);
        vec[i] = std::pair<uint64_t, uint64_t>(i, sa[minPos]);
    }

    std::cout << "\r"
              << "Sorting minimal substrings...[5/6]" << std::flush;
    auto &inter = intervals;
    sort(vec.begin(), vec.end(), [&inter](const std::pair<uint64_t, uint64_t> &x, const std::pair<uint64_t, uint64_t> &y) {
        if (x.second == y.second)
        {
            return inter[x.first].lcp > inter[y.first].lcp;
        }
        else
        {
            return x.second < y.second;
        }
    });

    std::cout << "\r"
              << "Sorting minimal substrings...[6/6]" << std::flush;
    for (uint64_t i = 0; i < vec.size(); i++)
    {
        outputSortedMinimumSubstrings.push(vec[i]);
    }
    std::cout << "[END]" << std::endl;
}
*/


} // namespace stool