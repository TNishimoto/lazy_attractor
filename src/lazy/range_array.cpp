#include <stack>
#include "sa_lcp.hpp"
#include "range_array.hpp"
using namespace std;
using namespace sdsl;

namespace stool{
void SAPositionToMSLeaf::checkRangeArray(vector<LCPInterval> &intervals, vector<uint64_t> &parents)
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

                std::cout << interval.toString() << std::endl;
                throw - 1;
            }
            if (interval.lcp > intervals[longestPos].lcp)
            {
                std::cout << "RangeArray Error" << std::endl;
                std::cout << longestPos << std::endl;
                std::cout << intervals[longestPos].toString() << std::endl;
                std::cout << interval.toString() << std::endl;

                throw - 1;
            }
        }
        if (i % 1000 == 0)
            std::cout << "\r"
                      << "checking RangeArray... : [" << i << "/" << (size) << "]" << std::flush;
    }
}
void SAPositionToMSLeaf::constructRangeArray(vector<LCPInterval> &intervals, vector<uint64_t> &parents, uint64_t textSize)
{
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

    parents.resize(parents.size() + textSize, UINT64_MAX);
    uint64_t size = intervals.size();
    for (uint64_t i = 0; i < intervals.size(); i++)
    {
        auto &interval = intervals[i];
        uint64_t left = interval.i;
        for (uint64_t x = 0; x < childrens[i].size(); x++)
        {
            auto &child = intervals[childrens[i][x]];
            for (uint64_t z = left; z < child.i; z++)
            {
                parents[z + size] = i;
            }
            left = child.j + 1;
        }
        for (uint64_t z = left; z <= interval.j; z++)
        {
            parents[z + size] = i;
        }
    }
    //checkRangeArray(intervals, parents);
}

void SAPositionToMSLeaf::construct(vector<LCPInterval> &intervals, vector<uint64_t> &parents, uint64_t textSize)
{
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

    vector<std::pair<uint64_t, uint64_t>> startingPositionVec;
    for (uint64_t i = 0; i < intervals.size(); i++)
    {
        auto &interval = intervals[i];
        uint64_t left = interval.i;
        for (uint64_t x = 0; x < childrens[i].size(); x++)
        {
            auto &child = intervals[childrens[i][x]];
            if (left < child.i)
            {
                startingPositionVec.push_back(std::pair<uint64_t, uint64_t>(left, i));
            }
            left = child.j + 1;
        }
        if (left <= interval.j)
        {
            startingPositionVec.push_back(std::pair<uint64_t, uint64_t>(left, i));
        }
    }

    sort(startingPositionVec.begin(), startingPositionVec.end(), [&](const std::pair<uint64_t, uint64_t> &x, const std::pair<uint64_t, uint64_t> &y) {
        return x.first < y.first;
    });

    uint64_t m = startingPositionVec.size();
    this->idVec.resize(m);
    this->startingPositions.resize(textSize);

    for (uint64_t i = 0; i < textSize; i++)
    {
        this->startingPositions[i] = 0;
    }

    for (uint64_t i = 0; i < m; i++)
    {
        this->idVec[i] = startingPositionVec[i].second;
        this->startingPositions[startingPositionVec[i].first] = 1;
    }
    rank_support_v<1> _bv_rank(&this->startingPositions);
    this->bv_rank.set_vector(&this->startingPositions);
    _bv_rank.swap(this->bv_rank);

    /*
    for (uint64_t i = 0; i < textSize; i++)
    {
        std::cout << i << "/" << this->bv_rank(i) << std::endl;
    }
    */
}
}