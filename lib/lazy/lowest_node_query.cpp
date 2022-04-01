#include <stack>
#include "../include/lazy/lowest_node_query.hpp"
using namespace std;
using namespace sdsl;

namespace stool
{
namespace lazy
{
void LowestNodeQuery::checkRangeArray(vector<LCPInterval<uint64_t>> &intervals, vector<uint64_t> &parents)
{
    uint64_t size = intervals.size();
    for (uint64_t i = 0; i < size; i++)
    {
        auto &interval = intervals[i];
        for (uint64_t x = interval.i; x <= interval.j; x++)
        {
            uint64_t index = x + size;
            uint64_t longestPos = parents[index];
            if (longestPos == UINT64_MAX)
            {
                std::cout << "RangeArray Error2" << std::endl;
                std::cout << x << std::endl;

                std::cout << interval.to_string() << std::endl;
                throw - 1;
            }
            if (interval.lcp > intervals[longestPos].lcp)
            {
                std::cout << "RangeArray Error" << std::endl;
                std::cout << longestPos << std::endl;
                std::cout << intervals[longestPos].to_string() << std::endl;
                std::cout << interval.to_string() << std::endl;

                throw - 1;
            }
        }
        if (i % 1000 == 0)
            std::cout << "\r"
                      << "checking RangeArray... : [" << i << "/" << (size) << "]" << std::flush;
    }
}
std::vector<LowestNodeInterval> LowestNodeQuery::constructLowestNodeIntervalVec(const std::vector<LCPInterval<uint64_t>> &intervals,const std::vector<uint64_t> &parents)
{
  std::vector<LowestNodeInterval> startingPositionVec;
  vector<vector<uint64_t>> childrens;
  childrens.resize(intervals.size());
  for (uint64_t i = 0; i < intervals.size(); i++)
  {
    if (parents[i] != UINT64_MAX)
    {
      childrens[parents[i]].push_back(i);
    }
  }

  for (uint64_t i = 0; i < intervals.size(); i++)
  {
    sort(childrens[i].begin(), childrens[i].end(), [&](const uint64_t &x, const uint64_t &y) {
      return intervals[x].i < intervals[y].i;
    });
  }

  //vector<std::pair<uint64_t, uint64_t>> startingPositionVec;
  for (uint64_t i = 0; i < intervals.size(); i++)
  {

    auto &interval = intervals[i];
    uint64_t left = interval.i;
    for (uint64_t x = 0; x < childrens[i].size(); x++)
    {
      auto &child = intervals[childrens[i][x]];
      if (left < child.i)
      {
        startingPositionVec.push_back(LowestNodeInterval(i,left));
      }
      left = child.j + 1;
    }
    if (left <= interval.j)
    {
      startingPositionVec.push_back(LowestNodeInterval(i,left));
    }
  }

  sort(startingPositionVec.begin(), startingPositionVec.end(), [&](const LowestNodeInterval &x, const LowestNodeInterval &y) {
    return x.rangeStartPosition < y.rangeStartPosition;
  });

  return startingPositionVec;
}

void LowestNodeQuery::construct(const vector<LCPInterval<uint64_t>> &intervals,const vector<uint64_t> &parents, uint64_t _textSize)
{
    this->textSize = _textSize;
    std::vector<LowestNodeInterval> startingPositionVec = constructLowestNodeIntervalVec(intervals,parents);

    uint64_t m = startingPositionVec.size();
    this->idVec.resize(m);
    this->startingPositions.resize(textSize);

    for (uint64_t i = 0; i < textSize; i++)
    {
        this->startingPositions[i] = 0;
    }

    for (uint64_t i = 0; i < m; i++)
    {
        this->idVec[i] = startingPositionVec[i].id;
        this->startingPositions[startingPositionVec[i].rangeStartPosition] = 1;
    }
    rank_support_v<1> _bv_rank(&this->startingPositions);
    this->bv_rank.set_vector(&this->startingPositions);
    _bv_rank.swap(this->bv_rank);

}

std::vector<uint64_t> LowestNodeQuery::constructLowestNodeIDVec(const std::unordered_set<uint64_t> &currentIntervals,const std::vector<LCPInterval<uint64_t>> &intervals, uint64_t textSize)
  {
    std::vector<uint64_t> r;
    r.resize(textSize, UINT64_MAX);
    std::vector<uint64_t> tmp;
    for (auto &it : currentIntervals)
    {
      tmp.push_back(it);
    }
    std::sort(tmp.begin(), tmp.end(), [&](uint64_t xi, uint64_t yi) {
      const LCPInterval<uint64_t> &x = intervals[xi];
      const LCPInterval<uint64_t> &y = intervals[yi];

      if (x.i == y.i)
      {
        if (x.j == y.j)
        {
          return x.lcp < y.lcp;
        }
        else
        {
          return x.j > y.j;
        }
      }
      else
      {
        return x.i < y.i;
      }
    });
    for (auto &it : tmp)
    {
      const LCPInterval<uint64_t> &interval = intervals[it];
      for (uint64_t x = interval.i; x <= interval.j; x++)
      {
        r[x] = it;
      }
    }
    return r;
  }

} // namespace lazy
} // namespace stool