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

/*
  This is a class to compute lazy attractors.
 */
class LazyAttractor
{
  private:
  /*
  Return the vector X of size n such that X[i] stores the minimal occurrence of the i-th minimal substring.
   */
  static std::vector<uint64_t> constructMinimalOccurrencesOfMinimalSubstrings(const std::vector<LCPInterval<uint64_t>> &intervals,const  std::vector<uint64_t> &parents, const std::vector<uint64_t> &sa);
  /*
  Return the stack storing minimal occurrences of the minimal substrings.
   */
  static std::stack<MinimalSubstringInfo> constructSortedMinimalOccurrenceStack(const std::vector<LCPInterval<uint64_t>> &intervals,const  std::vector<uint64_t> &parents,const  std::vector<uint64_t> &sa);

  /*
  Remove minimal substrings captured by the given last attractor from the dynamic interval tree.
   */
  static bool removeMSIntervalsCapturedByTheLastAttractor(TINDEX lastAttractor,const  std::vector<LCPInterval<uint64_t>> &intervals,const  std::vector<uint64_t> &isa, DynamicIntervalTree &tree, std::stack<MinimalSubstringInfo> &sortedMinimumSubstrings);
  
  public:

  /*
    Return the vector storing lazy attractors of the given text.
   */
template<typename CHAR>
  static std::vector<uint64_t>  computeLazyAttractors(std::vector<CHAR> &text, std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents);
  /*
    Return the vector storing lazy attractors of the given text.
   */
template<typename CHAR>
  static std::vector<uint64_t>  computeLazyAttractors(std::vector<CHAR> &text, std::vector<uint64_t> &sa, std::vector<uint64_t> &isa  ,std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents);
  /*
    Return the vector storing lazy attractors of the given text.
   */
template<typename CHAR>
  static std::vector<uint64_t> naiveComputeLazyAttractors(std::vector<CHAR> &text, std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals);
};

} // namespace lazy
} // namespace stool