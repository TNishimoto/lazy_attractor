#pragma once
//#include "io.h"
#include <stack>
//#include "uftree.hpp"
#include "stool/src/sa_bwt_lcp.hpp"
#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
using namespace sdsl;

namespace stool
{
namespace lazy
{
using SINDEX = uint64_t;
using TINDEX = uint64_t;

// This data structure supports the function which
// receives a position i on suffix array and returns the leaf ID containing i in minimal substring tree.
class LeafRankDataStructure
{
  std::vector<uint64_t> idVec;
  bit_vector startingPositions;
  rank_support_v<1> bv_rank;

  static void checkRangeArray(std::vector<LCPInterval<uint64_t>> &intervals, std::vector<uint64_t> &parents);
  static void constructRangeArray(std::vector<LCPInterval<uint64_t>> &intervals, std::vector<uint64_t> &parents, uint64_t textSize);

public:
  LeafRankDataStructure()
  {
  }

  void construct(std::vector<LCPInterval<uint64_t>> &intervals, std::vector<uint64_t> &parents, uint64_t textSize);
  // Return the leaf ID containing pos in minimal substring tree.
  uint64_t getLeafID(SINDEX pos)
  {
    return this->idVec[bv_rank(pos + 1) - 1];
  }
  // Construct the array such that outputVec[i] stores the leftmost occurrence of the $i$-th minimal substring.
  std::vector<uint64_t> constructMinimalOccurrenceVec(std::vector<uint64_t> &sa, uint64_t intervalCount)
  {
    std::vector<uint64_t> outputVec;
    outputVec.resize(intervalCount, UINT64_MAX);
    int64_t p = -1;
    for (int64_t i = 0; i < (int64_t)this->startingPositions.size(); i++)
    {
      if (this->startingPositions[i] == 1)
      {
        p++;
      }
      uint64_t index = sa[i];
      if (index < outputVec[this->idVec[p]])
      {
        outputVec[this->idVec[p]] = index;
      }
    }
    return outputVec;
  }
};
} // namespace lazy
} // namespace stool