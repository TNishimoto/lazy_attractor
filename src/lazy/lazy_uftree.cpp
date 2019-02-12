#include <stack>
#include "sa_lcp.hpp"
#include "lazy_uftree.hpp"
namespace stool
{

LazyUFTree::LazyUFTree(vector<LCPInterval> &_intervals, vector<uint64_t> &_parents, uint64_t textSize) : intervals(_intervals), parents(_parents)
{

    std::cout << "construct rangeArray" << std::endl;
    rangeArray.construct(_intervals, _parents, textSize);

    this->removeVec.resize(_intervals.size() - 1, false);

    std::cout << "Constructing UFTree..." << std::flush;
    this->uftree.initialize(_parents);
    std::cout << "[END]" << std::endl;
}

uint64_t LazyUFTree::getTreeNodeID(SINDEX pos)
{
    //uint64_t x = this->isa[pos];
    return this->rangeArray.getParentMSIntervalID(pos);
}
uint64_t LazyUFTree::getLongestLCPIntervalID(SINDEX pos)
{
    uint64_t id = this->getTreeNodeID(pos);
    while (!this->uftree.isRoot(id))
    {
        if (this->checkRemovedInterval(id))
        {
            //id = this->uftree.getParent(id);
            uint64_t parent = this->uftree.getParent(id);
            if (this->checkRemovedInterval(parent))
            {
                uint64_t p = this->uftree.unionParent(id);
                assert(p != UINT64_MAX);
            }
            else
            {
                return parent;
            }
        }
        else
        {
            return id;
        }
    }
    return UINT64_MAX;
}

bool LazyUFTree::removeLongestLCPInterval(SINDEX pos)
{
    uint64_t id = this->getTreeNodeID(pos);
    if (this->removeVec[id])
    {
        uint64_t result = this->uftree.unionParent(id);
        bool b = result != UINT64_MAX;
        if (b)
        {
            this->removeVec[id] = true;
        }
        return b;
    }
    else
    {
        this->removeVec[id] = true;
        return true;
    }
}
bool LazyUFTree::checkRemovedInterval(uint64_t intervalID)
{
    return this->removeVec[intervalID] || this->uftree.checkMerge(intervalID);
}
uint64_t LazyUFTree::removeMSIntervals(TINDEX pos, vector<uint64_t> &isa, TINDEX lastAttractor)
{
    while (true)
    {
        uint64_t id = getLongestLCPIntervalID(isa[pos]);
        if (id == UINT64_MAX)
        {
            return id;
        }
        else
        {
            uint64_t longestIntervalLength = intervals[id].lcp;
            if (pos + longestIntervalLength - 1 >= lastAttractor)
            {
                this->removeLongestLCPInterval(isa[pos]);
            }
            else
            {
                return id;
            }
        }
    }
    return UINT64_MAX;
}

void LazyUFTree::constructSortedMinimumSubstrings(vector<uint64_t> &sa, stack<std::pair<uint64_t, uint64_t>> &outputSortedMinimumSubstrings)
{
    vector<uint64_t> vec;
    //vec.resize(intervals.size(), UINT64_MAX);
    vector<uint64_t> &parents = this->parents;

    this->rangeArray.constructMinimalOccurrenceVec(sa, intervals.size(), vec);
    /*
    int64_t p = -1;
    for (int64_t i = 0; i < this->rangeArray.startingPositions.size(); i++)
    {
        if (this->rangeArray.startingPositions[i] == 1)
        {
            p++;
        }
        uint64_t index = sa[i];
        if (index < vec[this->rangeArray.idVec[p]])
        {
            vec[this->rangeArray.idVec[p]] = index;
        }
    }
    */

    for (int64_t i = this->intervals.size() - 1; i >= 0; i--)
    {
        assert(vec[i] != UINT64_MAX);
        if (parents[i] != UINT64_MAX)
        {
            if (vec[i] < vec[parents[i]])
            {
                vec[parents[i]] = vec[i];
            }
        }
    }

    vector<std::pair<uint64_t, uint64_t>> vec2;
    vec2.resize(vec.size());

    for (uint64_t i = 0; i < vec.size(); i++)
    {
        vec2[i] = std::pair<uint64_t, uint64_t>(i, vec[i]);
    }

    sort(vec2.begin(), vec2.end(), [&](const std::pair<uint64_t, uint64_t> &x, const std::pair<uint64_t, uint64_t> &y) {
        if (x.second == y.second)
        {
            return intervals[x.first].lcp > intervals[y.first].lcp;
        }
        else
        {
            return x.second < y.second;
        }
    });

    for (uint64_t i = 0; i < vec2.size(); i++)
    {
        outputSortedMinimumSubstrings.push(vec2[i]);
    }
}
void LazyUFTree::computeAttractors(string &text, vector<LCPInterval> &_intervals, vector<uint64_t> &_parents, vector<uint64_t> &outputAttrs)
{
    vector<uint64_t> sa, isa;
    stack<std::pair<uint64_t, uint64_t>> sortedMinimumSubstrings;
    constructSA(text, sa, isa);

    LazyUFTree lufTree(_intervals, _parents, text.size());
    //lufTree.initialize;
    lufTree.constructSortedMinimumSubstrings(sa, sortedMinimumSubstrings);

    uint64_t n = sa.size();
    uint64_t prevAttractorPosition = UINT64_MAX;
    for (int64_t i = n - 1; i >= 0; i--)
    {

        if (i % 100000 == 0)
            std::cout << "\r"
                      << "Computing lazy attractors... : [" << (n - i) << "/" << n << "]" << std::flush;
        lufTree.removeMSIntervals(i, isa, prevAttractorPosition);

        while (sortedMinimumSubstrings.size() > 0)
        {
            auto top = sortedMinimumSubstrings.top();
            if (top.second == i)
            {
                if (!lufTree.checkRemovedInterval(top.first))
                {
                    outputAttrs.push_back(i);
                    prevAttractorPosition = i;
                    lufTree.removeMSIntervals(i, isa, prevAttractorPosition);

                    //this->addAttractor(i, outputAttrs);
                }

                sortedMinimumSubstrings.pop();
            }
            else
            {
                break;
            }
        }
    }
    std::cout << std::endl;

    sort(outputAttrs.begin(), outputAttrs.end());
}

} // namespace stool