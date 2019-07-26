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
  static void constructMSIntervalParents(std::vector<LCPInterval> &intervals, std::vector<uint64_t> &outputParents);

public:
  std::vector<LCPInterval> nodes;
  std::vector<uint64_t> parents;
  // The outputIntervals stores lcp intervals of minimal substring.
  // The lcp intervals sorted in lexcographically order.
  // The i-th element in the outputParents stores the index of the parent of the node representing the i-th minimal substring.
  static void construct(std::string &text, std::vector<LCPInterval> &outputIntervals, std::vector<uint64_t> &outputParents);
  void construct(std::string &text)
  {
    MinimalSubstringTree::construct(text, this->nodes, this->parents);
  }
  /*
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
  */
  void write(std::string filepath, std::string &text);
  void load(std::string filepath, std::string &text);
  void loadOrConstruct(std::string filepath, std::string *text);
};

} // namespace stool
