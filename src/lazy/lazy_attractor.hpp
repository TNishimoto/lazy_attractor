#pragma once
//#include "stool/include/io.hpp"
#include <stack>
#include "stool/include/sa_bwt_lcp.hpp"
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

  static uint64_t naiveComputeNextLazyAttractor(std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals){

    int64_t wholeFstOcc = -1;
    for (uint64_t i = 0; i < intervals.size(); i++)
    {
        stool::LCPInterval<uint64_t> &interval = intervals[i];
        if (interval.lcp == 0)
            continue;

        int64_t fstOcc = sa.size();
        for (uint64_t x = interval.i; x <= interval.j; x++)
        {
            int64_t pos = sa[x];
            if (pos < fstOcc)
                fstOcc = pos;
        }
        if(wholeFstOcc < fstOcc) wholeFstOcc = fstOcc;
    }
    return wholeFstOcc == -1 ? UINT64_MAX : wholeFstOcc;
  }

};

} // namespace lazy
} // namespace stool