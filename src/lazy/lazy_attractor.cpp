#include "io.h"
#include <stack>
#include "sa_lcp.hpp"
#include "mstree.hpp"
#include "print.hpp"

#include "lazy_attractor.hpp"
using namespace std;

namespace stool
{

void LazyAttractorAlgorithm::checkRangeArray(vector<LCPInterval> &intervals, vector<uint64_t> &parents)
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
void LazyAttractorAlgorithm::constructRangeArray(vector<LCPInterval> &intervals, vector<uint64_t> &parents, uint64_t textSize)
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
uint64_t LazyAttractorAlgorithm::getTextSize()
{
    return this->sa.size();
}
uint64_t LazyAttractorAlgorithm::getTreeNodeID(uint64_t pos)
{
    uint64_t x = this->isa[pos];
    return this->intervals.size() + x;
}
uint64_t LazyAttractorAlgorithm::getLongestLCPIntervalID(uint64_t pos)
{
    uint64_t id = this->getTreeNodeID(pos);
    while (!this->uftree.isRoot(id))
    {
        uint64_t parent = this->uftree.getParent(id);
        if (this->uftree.checkMerge(parent))
        {
            uint64_t p = this->uftree.unionParent(id);
            //std::cout << id << "/" << p << std::endl;
            if (p == UINT64_MAX){
                std::cout << "error: getLongestLCPIntervalID" << std::endl;
                throw - 1;
            }
        }
        else
        {
            assert(this->intervals[parent].i <= this->isa[pos] && this->isa[pos] <= this->intervals[parent].j);
            return parent;
        }
    }
    return UINT64_MAX;
}
bool LazyAttractorAlgorithm::removeLongestLCPInterval(uint64_t pos)
{
    uint64_t id = this->getTreeNodeID(pos);
    //uint64_t parent = this->getLongestLCPIntervalID(pos);
    uint64_t result = this->uftree.unionParent(id);
    //if(result != UINT64_MAX) std::cout << "remove: pos = " << pos << "[" << this->intervals[parent].i << ", " << this->intervals[parent].j << "]" << std::endl;
    return result != UINT64_MAX;
}
bool LazyAttractorAlgorithm::checkRemovedInterval(uint64_t intervalID)
{
    return this->uftree.checkMerge(intervalID);
}
void LazyAttractorAlgorithm::addAttractor(uint64_t pos, vector<uint64_t> &outputAttrs)
{
    assert(this->getLongestLCPIntervalID(pos) != UINT64_MAX);
    assert(outputAttrs.size() == 0 || outputAttrs[outputAttrs.size() - 1] != pos);
    outputAttrs.push_back(pos);

    //uint64_t checker = 0;
    while (true)
    {
        bool b = this->removeLongestLCPInterval(pos);
        //checker++;
        //std::cout << pos << "/";
        //if (checker > 100)
        //    throw - 1;
        if (!b)
            break;
    }
}
void LazyAttractorAlgorithm::constructSortedMinimumSubstrings()
{

    std::cout << "\r"
              << "Sorting minimal substrings...[1/6]" << std::flush;
    vector<std::pair<uint64_t, uint64_t>> vec;
    vec.resize(this->intervals.size());

    uint64_t n = this->sa.size();
    int_vector<> v(n, INT64_MAX);
    std::cout << "\r"
              << "Sorting minimal substrings...[2/6]" << std::flush;

    uint64_t m = 0;
    for (uint64_t i = 0; i < n; i++)
    {
        v[i] = this->sa[i];
    }
    std::cout << "\r"
              << "Sorting minimal substrings...[3/6]" << std::flush;
    rmq_succinct_sada<> rmq(&v);

    std::cout << "\r"
              << "Sorting minimal substrings...[4/6]" << std::flush;
    for (uint64_t i = 0; i < this->intervals.size(); i++)
    {
        uint64_t minPos = rmq(this->intervals[i].i, this->intervals[i].j);
        vec[i] = std::pair<uint64_t, uint64_t>(i, this->sa[minPos]);
    }

    std::cout << "\r"
              << "Sorting minimal substrings...[5/6]" << std::flush;
    auto &inter = this->intervals;
    sort(vec.begin(), vec.end(), [&inter](const std::pair<uint64_t, uint64_t> &x, const std::pair<uint64_t, uint64_t> &y) {
        if (x.second == y.second)
        {
            return inter[x.first].lcp > inter[y.first].lcp;
        }
        else
        {
            return x.second < y.second;
        }
    });

    std::cout << "\r"
              << "Sorting minimal substrings...[6/6]" << std::flush;
    for (uint64_t i = 0; i < vec.size(); i++)
    {
        this->sortedMinimumSubstrings.push(vec[i]);
    }
    std::cout << "[END]" << std::endl; 
}

LazyAttractorAlgorithm::LazyAttractorAlgorithm(string &text, vector<LCPInterval> &_intervals, vector<uint64_t> &_parents)
{
    constructSA(text, this->sa, this->isa);

    std::move(_intervals.begin(), _intervals.end(), back_inserter(this->intervals));
    constructRangeArray(_intervals, _parents, text.size());

    std::cout  << "Constructing UFTree..."  << std::flush;
    this->uftree.initialize(_parents);
    std::cout << "[END]" << std::endl;
    this->constructSortedMinimumSubstrings();
}
void LazyAttractorAlgorithm::compute(vector<uint64_t> &outputAttrs)
{
    uint64_t n = this->getTextSize();
    uint64_t prevAttractorPosition = UINT64_MAX;
    for (int64_t i = n - 1; i >= 0; i--)
    {

        if (i % 100000 == 0)
            std::cout << "\r"
                      << "Computing lazy attractors... : [" << (n - i) << "/" << n << "]" << std::flush;
        while (true)
        {
            uint64_t id = getLongestLCPIntervalID(i);
            if (id == UINT64_MAX)
            {
                break;
            }
            else
            {
                uint64_t longestIntervalLength = intervals[id].lcp;
                if (i + longestIntervalLength - 1 >= prevAttractorPosition)
                {
                    //std::cout << "prevAtt: " << prevAttractorPosition << ", len = " << longestIntervalLength<< std::endl;
                    this->removeLongestLCPInterval(i);
                }
                else
                {
                    break;
                }
            }
        }
        while (this->sortedMinimumSubstrings.size() > 0)
        {
            auto top = this->sortedMinimumSubstrings.top();
            if (top.second == i)
            {
                if (!this->checkRemovedInterval(top.first))
                {
                    this->addAttractor(i, outputAttrs);
                    prevAttractorPosition = i;
                }

                if (!this->checkRemovedInterval(top.first))
                {
                    std::cout << "error!!" << std::endl;
                    std::cout << this->intervals[top.first].toString() << std::endl;
                    auto p = getLongestLCPIntervalID(i);

                    std::cout << "isa: " << this->isa[i] << std::endl;

                    uint64_t pid = this->getTreeNodeID(i);
                    std::cout << "tree of " << pid << "/" << this->intervals.size() << std::endl;
                    while (this->uftree.parentVec[pid] != UINT64_MAX)
                    {
                        pid = this->uftree.parentVec[pid];
                        std::cout << this->intervals[pid].toString() << std::endl;
                    }

                    std::cout << p << std::endl;
                    std::cout << this->intervals[p].toString() << std::endl;

                    throw - 1;
                }

                this->sortedMinimumSubstrings.pop();
            }
            else
            {
                break;
            }
        }
    }
    std::cout << std::endl;

    sort(outputAttrs.begin(), outputAttrs.end());

    std::cout << "size = " << this->sortedMinimumSubstrings.size() << std::endl;
    for (uint64_t i = 0; i < this->intervals.size(); i++)
    {
        bool b = this->checkRemovedInterval(i);
        if (!b)
        {
            std::cout << "error" << std::endl;
            std::cout << this->intervals[i].toString() << std::endl;
        }
    }
}
} // namespace stool