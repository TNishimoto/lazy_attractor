#include <stack>
#include "stool/src/sa_bwt_lcp.hpp"
#include "lazy_uftree.hpp"
namespace stool
{
namespace lazy
{
LazyUFTree::LazyUFTree(vector<LCPInterval<uint64_t>> &_intervals, vector<uint64_t> &_parents, uint64_t textSize) : intervals(_intervals), parents(_parents)
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

#ifdef DEBUG
                uint64_t p = this->uftree.unionParent(id);
                assert(p != UINT64_MAX);
#else
                this->uftree.unionParent(id);
#endif
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
    assert(!this->checkRemovedInterval(id));
    return id;
    //return UINT64_MAX;
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

std::stack<MinimalSubstringInfo> LazyUFTree::constructSortedMinimumSubstrings(vector<uint64_t> &sa)
{
    stack<MinimalSubstringInfo> outputSortedMinimumSubstrings;
    //vec.resize(intervals.size(), UINT64_MAX);
    vector<uint64_t> &parents = this->parents;

    vector<uint64_t> minimalOccurrenceVec = this->rangeArray.constructMinimalOccurrenceVec(sa, intervals.size());

    for (int64_t i = this->intervals.size() - 1; i >= 0; i--)
    {
        assert(minimalOccurrenceVec[i] != UINT64_MAX);
        if (parents[i] != UINT64_MAX)
        {
            if (minimalOccurrenceVec[i] < minimalOccurrenceVec[parents[i]])
            {
                minimalOccurrenceVec[parents[i]] = minimalOccurrenceVec[i];
            }
        }
    }

    vector<MinimalSubstringInfo> sortedMinimumSubstringVec;
    sortedMinimumSubstringVec.resize(minimalOccurrenceVec.size());

    for (uint64_t i = 0; i < minimalOccurrenceVec.size(); i++)
    {
        sortedMinimumSubstringVec[i] = MinimalSubstringInfo(i, minimalOccurrenceVec[i]);
    }

    sort(sortedMinimumSubstringVec.begin(), sortedMinimumSubstringVec.end(), [&](const MinimalSubstringInfo &x, const MinimalSubstringInfo &y) {
        if (x.minOcc == y.minOcc)
        {
            return intervals[x.id].lcp > intervals[y.id].lcp;
        }
        else
        {
            return x.minOcc < y.minOcc;
        }
    });

    for (uint64_t i = 0; i < sortedMinimumSubstringVec.size(); i++)
    {
        if(intervals[sortedMinimumSubstringVec[i].id].lcp != 0){
            outputSortedMinimumSubstrings.push(sortedMinimumSubstringVec[i]);
        }
    }
    return outputSortedMinimumSubstrings;
}
void LazyUFTree::computeAttractors(vector<uint8_t> &text, vector<LCPInterval<uint64_t>> &_intervals, vector<uint64_t> &_parents, vector<uint64_t> &outputAttrs)
{
    vector<uint64_t> sa = constructSA(text);
    vector<uint64_t> isa = constructISA(text, sa);

    LazyUFTree lufTree(_intervals, _parents, text.size());
    //lufTree.initialize;
    stack<MinimalSubstringInfo> sortedMinimumSubstrings = lufTree.constructSortedMinimumSubstrings(sa);
    
    while(sortedMinimumSubstrings.size() > 0){
        auto top = sortedMinimumSubstrings.top();
        if(outputAttrs.size() > 0){
            assert(outputAttrs[outputAttrs.size()-1] != top.minOcc);
        }

        outputAttrs.push_back(top.minOcc);
        //std::cout << top.minOcc << std::endl;
        assert(lufTree.getLongestLCPIntervalID(isa[top.minOcc]) != UINT64_MAX);        
        lufTree.removeMSIntervalsCapturedByTheLastAttractor(isa, top.minOcc, sortedMinimumSubstrings);
    }
    
    /*
    uint64_t n = sa.size();
    uint64_t prevAttractorPosition = UINT64_MAX;
    for (int64_t i = n - 1; i >= 0; i--)
    {
        //assert(i >= n-1);
        if (i % 100000 == 0)
            std::cout << "\r"
                      << "Computing lazy attractors... : [" << (n - i) << "/" << n << "]" << std::flush;
        lufTree.removeMSIntervals(i, isa, prevAttractorPosition);
        

        while (sortedMinimumSubstrings.size() > 0)
        {
            auto top = sortedMinimumSubstrings.top();
            if ((int64_t)top.minOcc == i)
            {
                if (!lufTree.checkRemovedInterval(top.id))
                {
                    #ifdef DEBUG_PRINT
                    std::cout << "attr: " << top.minOcc << std::endl;
                    #endif
                    outputAttrs.push_back(i);
                    prevAttractorPosition = i;
                    lufTree.removeMSIntervals(i, isa, prevAttractorPosition);

                    //this->addAttractor(i, outputAttrs);
                }
                assert(lufTree.checkRemovedInterval(top.id));

                sortedMinimumSubstrings.pop();
            }
            else
            {
                break;
            }
        }
    }
    */
    
    std::cout << std::endl;

    sort(outputAttrs.begin(), outputAttrs.end());
}
} // namespace lazy
} // namespace stool