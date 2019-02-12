#pragma once
#include "lcp_interval.hpp"
#include "sa_lcp.hpp"
#include <queue>
#include <stack>

using namespace sdsl;
using namespace std;

namespace stool
{

// This class has information about a node in the suffix tree of an input string. 
// This class stores the interval [i..j] of the node on SA, and the length of the edge between the parent and the node.
class SpecializedLCPInterval
{
public:
  uint64_t i;
  uint64_t j;
  uint64_t parentEdgeLength;
  SpecializedLCPInterval()
  {
  }
  SpecializedLCPInterval(uint64_t _i, uint64_t _j, uint64_t _lcp) : i(_i), j(_j), parentEdgeLength(_lcp)
  {
  }

  string toString()
  {
    return "[" + to_string(i) + ", " + to_string(j) + ", " + to_string(parentEdgeLength) + "]";
  }
};

// Enumerate LCP intervals in the suffix tree of an input text in post-order.
class OnlineLCPInterval
{
  std::stack<LCPInterval> stack;
  std::stack<std::pair<uint64_t, uint64_t>> stack2;

  vector<uint64_t> &sa;
  vector<uint64_t> &lcp;
  int64_t n = 0;
  LCPInterval oFstInterval;
  std::queue<LCPInterval> queue;
  uint64_t intervalCount = 0;
  std::queue<SpecializedLCPInterval> outputQueue;

  bool next();
  bool nextLCPIntervalWithParent();
  bool nextLCPInterval();
  void testcompute(string &text, vector<LCPInterval> &outputIntervals);
public:
  uint64_t counter_i = 0;
  OnlineLCPInterval(vector<uint64_t> &_sa, vector<uint64_t> &_lcp);
  // Return the next LCP interval of the LCP interval returned immediately before, and true.
  // If the interval returned immediately before is the last interval, it returns false.
  bool takeFront(SpecializedLCPInterval &outputInterval);
};
} // namespace stool