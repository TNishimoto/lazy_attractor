#pragma once
#include "io.h"
#include <stack>
#include "uftree.hpp"
#include "sa_lcp.hpp"
#include "lcp_interval.hpp"
#include "range_array.hpp"
using namespace std;
using namespace sdsl;

namespace stool
{

class LazyUFTree{
  private:
  UFTree uftree;
  vector<bool> removeVec;
  uint64_t getLongestLCPIntervalID(SINDEX pos);
  uint64_t getTreeNodeID(SINDEX pos);
  bool removeLongestLCPInterval(SINDEX pos);
  vector<LCPInterval> &intervals;
  vector<uint64_t> &parents;

  SAPositionToMSLeaf rangeArray;

  bool checkRemovedInterval(uint64_t intervalID);
  uint64_t removeMSIntervals(TINDEX pos, vector<uint64_t> &isa, TINDEX lastAttractor);
  void constructSortedMinimumSubstrings(vector<uint64_t> &sa, stack<std::pair<uint64_t, uint64_t>> &outputSortedMinimumSubstrings);

  public:
  LazyUFTree(vector<LCPInterval> &_intervals, vector<uint64_t> &_parents, uint64_t textSize);
  void initialize(vector<LCPInterval> &_intervals, vector<uint64_t> &_parents, uint64_t textSize);
  static void computeAttractors(string &text, vector<LCPInterval> &_intervals, vector<uint64_t> &_parents, vector<uint64_t> &outputAttrs);

};
}