#pragma once
#include <iostream>
#include <string>
#include <memory>

#include "lcp_interval.hpp"

using namespace sdsl;
using namespace std;
using namespace stool;

namespace stool
{

class OfflineSuffixTree
{
  public:
    vector<LCPInterval> intervals;
    vector<uint64_t> parents;
    vector<uint64_t> sa;
    vector<uint64_t> suffixLinks;
    vector<vector<std::pair<char, uint64_t>>> childrens;
    string *text;

    uint64_t isRoot(uint64_t node);
    uint64_t getRoot();

    uint64_t getEdgeStartingPosition(uint64_t node);
    uint64_t getEdgeLength(uint64_t node);
    char getFirstChar(uint64_t node);
    uint64_t getChild(uint64_t node, char c);

    uint64_t size();

    void constructChars();
    uint64_t computeSuffixLink(uint64_t node);
    uint64_t getLeaveNum(uint64_t node);
    uint64_t getDegree(uint64_t node);
    uint64_t selectChild(uint64_t node, uint64_t i);

    void constructSuffixLinks();

    void initialize(string &_text);
};

} // namespace stool