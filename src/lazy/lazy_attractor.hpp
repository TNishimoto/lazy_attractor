#pragma once
//#include "stool/src/io.h"
#include <stack>
#include "stool/src/sa_bwt_lcp.hpp"
#include "dynamic_interval_tree.hpp"
using namespace sdsl;

namespace stool
{
namespace lazy
{

class LazyAttractor
{
public:
  static bool removeMSIntervalsCapturedByTheLastAttractor(TINDEX lastAttractor, std::vector<LCPInterval<uint64_t>> &intervals, std::vector<uint64_t> &isa, DynamicIntervalTree &tree, std::stack<MinimalSubstringInfo> &sortedMinimumSubstrings);
  
  static void computeAttractors(std::vector<uint8_t> &text, std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents, std::vector<uint64_t> &outputAttrs);
  
};

} // namespace lazy
} // namespace stool