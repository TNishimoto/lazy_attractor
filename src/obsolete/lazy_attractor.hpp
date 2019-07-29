#pragma once
#include "io.h"
#include <stack>
#include "uftree.hpp"
#include "sa_lcp.hpp"
#include "lcp_interval.hpp"
#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include "range_array.hpp"
#include "lazy_uftree.hpp"
using namespace std;
using namespace sdsl;

namespace stool
{
class LazyAttractorAlgorithm
{
  /*
  DynamicIntervalTree lufTree;
  stack<std::pair<uint64_t, uint64_t>> sortedMinimumSubstrings;
  vector<uint64_t> sa;
  vector<uint64_t> isa;
  */
  //uint64_t getTextSize();
  //static void constructSortedMinimumSubstrings(vector<LCPInterval> &intervals, vector<uint64_t> &sa,stack<std::pair<uint64_t, uint64_t>> &outputSortedMinimumSubstrings);
  //static void constructSortedMinimumSubstrings(vector<LCPInterval> &intervals, vector<uint64_t> &parents, vector<uint64_t> &sa, RangeArrayForUFTree &rangeArray,stack<std::pair<uint64_t, uint64_t>> &outputSortedMinimumSubstrings);

public:

  //LazyAttractorAlgorithm(string &text, vector<LCPInterval> &_intervals, vector<uint64_t> &_parents);
  //static void run(string &text, vector<LCPInterval> &_intervals, vector<uint64_t> &_parents, vector<uint64_t> &outputAttrs);
};
} // namespace stool