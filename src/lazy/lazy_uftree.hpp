#pragma once
//#include "stool/src/io.h"
#include <stack>
#include "uftree.hpp"
#include "stool/src/sa_bwt_lcp.hpp"

#include "range_array.hpp"
using namespace sdsl;

namespace stool
{
namespace lazy
{

struct MinimalSubstringInfo
{
  uint64_t id;
  uint64_t minOcc;
  MinimalSubstringInfo(){

  };
  MinimalSubstringInfo(uint64_t _id, uint64_t _minOcc) : id(_id), minOcc(_minOcc)
  {
  }
};

class LazyUFTree
{
private:
  UFTree uftree;
  std::vector<bool> removeVec;
  std::vector<LCPInterval<uint64_t>> &intervals;
  std::vector<uint64_t> &parents;
  SAPositionToMSLeaf rangeArray;

  uint64_t getLongestLCPIntervalID(SINDEX pos);
  uint64_t getTreeNodeID(SINDEX pos);
  bool removeLongestLCPInterval(SINDEX pos);

  bool checkRemovedInterval(uint64_t intervalID);
  uint64_t removeMSIntervals(TINDEX pos, std::vector<uint64_t> &isa, TINDEX lastAttractor);
  std::stack<MinimalSubstringInfo> constructSortedMinimumSubstrings(std::vector<uint64_t> &sa);

  bool removeMSIntervalsCapturedByTheLastAttractor(vector<uint64_t> &isa, TINDEX lastAttractor, stack<MinimalSubstringInfo> &sortedMinimumSubstrings)
  {
    int64_t currentPos = (int64_t)lastAttractor;
    while (sortedMinimumSubstrings.size() > 0)
    {

      uint64_t id = getLongestLCPIntervalID(isa[currentPos]);
      bool noMSIntervalCapturedByTheLastAttractorOnTheCurrentPos = false;
      assert(id != UINT64_MAX);
      uint64_t longestIntervalLength = intervals[id].lcp;
      if (longestIntervalLength > 0 && currentPos + longestIntervalLength - 1 >= lastAttractor)
      {
        #ifdef DEBUG
        bool xb = this->removeLongestLCPInterval(isa[currentPos]);
        assert(xb);
        #else
        this->removeLongestLCPInterval(isa[currentPos]);
        #endif
      }
      else
      {
        noMSIntervalCapturedByTheLastAttractorOnTheCurrentPos = true;
      }

      /*
       Remove minimal substrings starting at positioin the current pos from the stack.       
       */
      if (noMSIntervalCapturedByTheLastAttractorOnTheCurrentPos)
      {
        while (sortedMinimumSubstrings.size() > 0)
        {
          auto top = sortedMinimumSubstrings.top();
          if (top.minOcc == (uint64_t)currentPos)
          {

            if (!this->checkRemovedInterval(top.id))
            {
              return true;
            }
            else
            {
              sortedMinimumSubstrings.pop();
            }
          }
          else
          {
            --currentPos;
            break;
          }
        }
      }
    }
    return false;
  }

public:
  LazyUFTree(std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents, uint64_t textSize);
  void initialize(std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents, uint64_t textSize);
  static void computeAttractors(vector<uint8_t> &text, std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents, std::vector<uint64_t> &outputAttrs);
};
} // namespace lazy
} // namespace stool