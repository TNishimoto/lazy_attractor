#pragma once
#include <iostream>
#include <string>
#include <memory>

#include "fmindex.hpp"
#include "lcp_interval.hpp"
#include "online_lcp_interval.hpp"
#include "online_ms_interval.hpp"

namespace stool
{

// The minimal substring tree of a string T is the trie for minimal substrings for T.
class MinimalSubstringTree
{
private:
  // Construct the outputParents using lcp intervals of minimal substring for T.
  static void constructMSIntervalParents(vector<LCPInterval> &intervals, vector<uint64_t> &outputParents);

public:
  vector<LCPInterval> nodes;
  vector<uint64_t> parents;
  // The outputIntervals stores lcp intervals of minimal substring.
  // The lcp intervals sorted in lexcographically order.
  // The i-th element in the outputParents stores the index of the parent of the node representing the i-th minimal substring.
  static void construct(string &text, vector<LCPInterval> &outputIntervals, vector<uint64_t> &outputParents);
  void construct(string &text)
  {
    MinimalSubstringTree::construct(text, this->nodes, this->parents);
  }
  void load(string filepath)
  {
    string filepath2 = filepath + ".parents";
    IO::load<LCPInterval>(filepath, nodes);
    IO::load<uint64_t>(filepath2, parents);
  }
  void write(string filepath)
  {
    string filepath2 = filepath + ".parents";

    IO::write(filepath, nodes);
    IO::write(filepath2, parents);
  }
  void loadOrConstruct(string &text, string filepath)
  {
    string filepath2 = filepath + ".parents";
    std::ifstream m_ifs(filepath);
    std::ifstream p_ifs(filepath2);
    bool mSubstrFileExist = m_ifs.is_open();
    bool mSubstrParentFileExist = p_ifs.is_open();
    if (!mSubstrFileExist || !mSubstrParentFileExist)
    {
      MinimalSubstringTree::construct(text, this->nodes, this->parents);
      IO::write(filepath, this->nodes);
      IO::write(filepath2, this->parents);
    }
    else
    {
      IO::load<LCPInterval>(filepath, this->nodes);
      IO::load<uint64_t>(filepath2, this->parents);
    }
  }
};

} // namespace stool
