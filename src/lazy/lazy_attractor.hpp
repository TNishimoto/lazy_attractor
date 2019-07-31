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
  private:
  /*
  Return minimal substrings on the given suffix array. The minimal substrings are sorted in the order of the their minimal occrrences.
   */
  static std::vector<uint64_t> constructMinimalOccurrenceVec(std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals, std::vector<uint64_t> &parents);
  static std::stack<MinimalSubstringInfo> constructSortedMinimalOccurrenceStack(std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals, std::vector<uint64_t> &parents);
  static bool removeMSIntervalsCapturedByTheLastAttractor(TINDEX lastAttractor, std::vector<LCPInterval<uint64_t>> &intervals, std::vector<uint64_t> &isa, DynamicIntervalTree &tree, std::stack<MinimalSubstringInfo> &sortedMinimumSubstrings);
  
  public:

  
  static std::vector<uint64_t>  computeLazyAttractors(std::vector<uint8_t> &text, std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents);
  static std::vector<uint64_t>  computeLazyAttractors(std::vector<uint8_t> &text, std::vector<uint64_t> &sa, std::vector<uint64_t> &isa  ,std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents);

  static std::vector<uint64_t> naiveComputeLazyAttractors(std::vector<uint8_t> &text, std::vector<LCPInterval<uint64_t>> &intervals);
};

} // namespace lazy
} // namespace stool