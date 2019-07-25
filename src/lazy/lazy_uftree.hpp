#pragma once
#include "io.h"
#include <stack>
#include "uftree.hpp"
#include "stool/src/sa_bwt_lcp.hpp"

#include "range_array.hpp"
//using namespace std;
using namespace sdsl;

namespace stool
{

class LazyUFTree{
  private:
  UFTree uftree;
  std::vector<bool> removeVec;
  uint64_t getLongestLCPIntervalID(SINDEX pos);
  uint64_t getTreeNodeID(SINDEX pos);
  bool removeLongestLCPInterval(SINDEX pos);
  std::vector<LCPInterval<uint64_t>> &intervals;
  std::vector<uint64_t> &parents;

  SAPositionToMSLeaf rangeArray;

  bool checkRemovedInterval(uint64_t intervalID);
  uint64_t removeMSIntervals(TINDEX pos, std::vector<uint64_t> &isa, TINDEX lastAttractor);
  void constructSortedMinimumSubstrings(std::vector<uint64_t> &sa, std::stack<std::pair<uint64_t, uint64_t>> &outputSortedMinimumSubstrings);

  public:
  LazyUFTree(std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents, uint64_t textSize);
  void initialize(std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents, uint64_t textSize);
  static void computeAttractors(vector<uint8_t> &text, std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents, std::vector<uint64_t> &outputAttrs);

};
}