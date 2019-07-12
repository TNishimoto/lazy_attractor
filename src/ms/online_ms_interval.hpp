#pragma once
#include "lcp_interval.hpp"
#include "online_lcp_interval.hpp"
#include "fmindex.hpp"

#include <queue>
#include <stack>

using namespace sdsl;
//using namespace std;

namespace stool
{

class SCharMapInfo
{
public:
  //uint64_t j;
  //The previous characters in the interval instance, i.e., Chars(bwt[i..j]), where interval = [i..j].
  std::unordered_set<uint8_t> set;
  SpecializedLCPInterval interval;
  SCharMapInfo()
  {
  }
};

// Enumerate minimal substrings of an input string.
class OnlineMSInterval
{
  std::unordered_map<uint64_t, SCharMapInfo> charMap;
  FMIndex &fmi;
  std::unordered_map<uint64_t, std::unordered_map<uint64_t, uint64_t>> omap;

  //uint64_t debug = 0;

  void insert(LCPInterval &interval);
  //void collectChars(SpecializedLCPInterval &interval);
  void findMSInterval(uint64_t i, std::unordered_set<uint8_t> &excludedChars);
  void getExcludedChars(SpecializedLCPInterval &interval, std::vector<uint64_t> &children, std::unordered_set<uint8_t> &outputExcludedChars);
  // Get the interval of each child of the node by the input interval
  void getChildren(SpecializedLCPInterval &interval, std::vector<uint64_t> &outputVec);
  void mergeMapInfo(SpecializedLCPInterval &interval, std::vector<uint64_t> &children);

  void addLCPInterval(SpecializedLCPInterval &interval);
  void constructMSIntervals(std::vector<LCPInterval> &outputIntervals);

public:
  OnlineMSInterval(FMIndex &_fmi) : fmi(_fmi)
  {
  }

  static void construct(std::vector<uint64_t> &sa, std::vector<uint64_t> &lcp, FMIndex &fmi, std::vector<LCPInterval> &outputIntervals)
  {
    OnlineLCPInterval oli = OnlineLCPInterval(sa, lcp);
    OnlineMSInterval omi = OnlineMSInterval(fmi);
    SpecializedLCPInterval sli;

    while (oli.takeFront(sli))
    {
      if (oli.counter_i % 100000 == 0)
        std::cout << "\r"
                  << "Computing minimal substrings... :[1/4][" << oli.counter_i << "/" << sa.size() << "]" << std::flush;
      omi.addLCPInterval(sli);
    }

    omi.constructMSIntervals(outputIntervals);
  }
};
} // namespace stool