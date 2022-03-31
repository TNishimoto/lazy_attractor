#include <stack>
#include "stool/include/sa_bwt_lcp.hpp"
#include "dynamic_interval_tree.hpp"

namespace stool
{
namespace lazy
{
DynamicIntervalTree::DynamicIntervalTree(const std::vector<LCPInterval<uint64_t>> &_intervals,const  std::vector<uint64_t> &_parents, uint64_t textSize) : intervals(_intervals), parents(_parents)
{

    if(textSize > 100000)std::cout << "Constructing the data structure for lowest node queries..." << std::endl;
    lowestNodeQuery.construct(intervals, parents, textSize);

    //this->removeVec.resize(_intervals.size(), false);

    if(textSize > 100000)std::cout << "Initializing Union-find tree" << std::flush;
    this->uftree.initialize(parents);
    if(textSize > 100000)std::cout << "[END]" << std::endl;
}
uint64_t DynamicIntervalTree::getLowestLCPIntervalID(SINDEX sa_index)
{
    uint64_t id = this->lowestNodeQuery.getLowestNodeID(sa_index);
    uint64_t clusterRootID = this->uftree.getClusterID(id);
    return clusterRootID;
}

bool DynamicIntervalTree::removeLowestLCPInterval(SINDEX sa_index)
{
    uint64_t leafID = this->getLowestLCPIntervalID(sa_index);
    //assert(!this->removeVec[leafID]);
    uint64_t result = this->uftree.unionParent(leafID);
    bool b = result != UINT64_MAX;

    /*
    if (b)
    {
        this->removeVec[leafID] = true;
    }
    */
    return b;
}
bool DynamicIntervalTree::hasInterval(uint64_t intervalID)
{
    uint64_t clusterID = this->uftree.getClusterID(intervalID);
    return clusterID == intervalID;
    //return this->removeVec[intervalID] || this->uftree.checkMerge(intervalID);
    //return this->removeVec[intervalID];
}

std::vector<uint64_t> DynamicIntervalTree::constructLowestNodeIDVec(uint64_t textSize){
    
    std::vector<uint64_t> r;
    r.resize(textSize, UINT64_MAX);
    for (uint64_t i = 0; i < textSize; i++)
    {
      r[i] = this->getLowestLCPIntervalID(i);
    }
    return r;
  }
} // namespace lazy
} // namespace stool