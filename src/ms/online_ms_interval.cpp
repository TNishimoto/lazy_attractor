#include "online_ms_interval.hpp"

using namespace sdsl;
using namespace std;

namespace stool
{

// interval : the interval of cV, where c is a character and V is the string by a node on the suffix tree.
void OnlineMSInterval::insert(LCPInterval &interval)
{
  if (interval.i != 0)
  {
    if (this->omap.find(interval.i) == this->omap.end())
    {
      this->omap.insert(std::pair<uint64_t, std::unordered_map<uint64_t, uint64_t>>(interval.i, std::unordered_map<uint64_t, uint64_t>()));
    }
    auto &omap2 = omap[interval.i];

    if (omap2.find(interval.j) == omap2.end())
    {
      omap2.insert(std::pair<uint64_t, uint64_t>(interval.j, interval.lcp));
    }
    else
    {
      // not excecuted.
      if (interval.lcp < omap2[interval.j])
      {
        omap2[interval.j] = interval.lcp;
      }
    }
  }
}
void OnlineMSInterval::getExcludedChars(SpecializedLCPInterval &interval, vector<uint64_t> &children, std::unordered_set<uint8_t> &outputExcludedChars)
{

  std::unordered_map<uint8_t, bool> map;
  if (interval.i == interval.j)
  {
    map[fmi.bwt[interval.i]] = true;
  }
  else
  {
    for(auto child : children){
      for (auto it : charMap[child].set)
      {
        if(map.find(it) == map.end()){
          map[it] = true;
        }else{
          if(map[it]){
            map[it] = false;
          }
        }
      }

    }
  }

  for(auto& it : map){
    if(it.second){
      outputExcludedChars.insert(it.first);
    }
  }
}
void OnlineMSInterval::addLCPInterval(SpecializedLCPInterval &interval)
{
    std::unordered_set<uint8_t> excludedChars;
    vector<uint64_t> children;
  if(interval.i != UINT64_MAX){
    this->getChildren(interval, children);
    this->getExcludedChars(interval, children, excludedChars);

    for(auto it : children){
      this->findMSInterval(it, excludedChars);
    }
    this->mergeMapInfo(interval, children);
  }else{
    for(auto it : this->charMap){
      this->findMSInterval(it.first, excludedChars);
    }

  }
}
void OnlineMSInterval::findMSInterval(uint64_t i, std::unordered_set<uint8_t> &excludedChars)
{
  auto &info = this->charMap[i];
    /* 
        std::cout << info.interval.toString() << std::endl;
        for (uint8_t c : excludedChars)
        {
          std::cout << (char) c << ",";
        }
        std::cout << std::endl;
        for (uint8_t c : info.set)
        {
          std::cout << (char) c << ",";
        }
        std::cout << std::endl;
        */


  if (info.set.size() != 1)
  {
    for (uint8_t it : info.set)
    {


      if (excludedChars.find(it) == excludedChars.end())
      {
        //this->debug++;
        uint64_t b = info.interval.i;
        uint64_t e = info.interval.j;
        this->fmi.backwardSearch(b, e, it);
        LCPInterval inte = LCPInterval(b, e, info.interval.parentEdgeLength == UINT64_MAX ? 1 : info.interval.parentEdgeLength + 2);
        insert(inte);
      }
    }
  }
}
void OnlineMSInterval::getChildren(SpecializedLCPInterval &interval, vector<uint64_t> &outputVec)
  {
    if (interval.i != interval.j)
    {
      auto &info = charMap[interval.i];
      uint64_t k = info.interval.i;
      while (k <= interval.j)
      {
        outputVec.push_back(k);
        k = this->charMap[k].interval.j + 1;
      }
    }
  }
  void OnlineMSInterval::mergeMapInfo(SpecializedLCPInterval &interval, vector<uint64_t> &children){

    if(children.size() == 0){
      //SCharMapInfo info;

      this->charMap[interval.i] = SCharMapInfo();
      auto &info = this->charMap[interval.i];


      info.interval = interval;
      info.set.insert(fmi.bwt[interval.i]);

    }else{
      auto &info = this->charMap[children[0]];
      info.interval = interval;
      for(uint64_t i=1;i<children.size();i++){
        for(auto c : this->charMap[children[i]].set){
          info.set.insert(c);          
        }
        this->charMap.erase(children[i]);
      }
    }

  }
void OnlineMSInterval::constructMSIntervals(vector<LCPInterval> &outputIntervals)
{
  SpecializedLCPInterval dummy(UINT64_MAX,UINT64_MAX, UINT64_MAX);
  this->addLCPInterval(dummy);

  /*
  std::cout << "" << std::endl;
  std::cout << "debug = " << this->debug << std::endl;
  std::cout << "" << std::endl;
  */

  vector<uint64_t> bVec;
  uint64_t u = 0;
  for (auto &it : this->omap)
  {
    if (u % 100000 == 0)
      std::cout << "\r"
                << "Computing minimal substrings... :[2/4][" << u << "/" << omap.size() << "]" << std::flush;
    u++;
    bVec.push_back(it.first);
  }
  sort(bVec.begin(), bVec.end());

  for (uint64_t i = 0; i < bVec.size(); i++)
  {

    if (i % 100000 == 0)
      std::cout << "\r"
                << "Computing minimal substrings... :[3/4][" << i << "/" << bVec.size() << "]" << std::flush;
    auto &sub = this->omap[bVec[i]];
    vector<uint64_t> eVec;
    for (auto &it : sub)
    {
      eVec.push_back(it.first);
    }
    sort(eVec.begin(), eVec.end(), [&](const uint64_t &x, const uint64_t &y) {
      return x > y;
    });
    for (uint64_t j = 0; j < eVec.size(); j++)
    {
      outputIntervals.push_back(LCPInterval(bVec[i] - 1, eVec[j] - 1, sub[eVec[j]]));
      //std::cout << LCPInterval(bVec[i] - 1, eVec[j] - 1, sub[eVec[j]]).toString() << std::endl;
    }
    this->omap.erase(bVec[i]);
  }
}

} // namespace stool