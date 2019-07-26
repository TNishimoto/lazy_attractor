#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include "lcp_interval.hpp"
#include "sa_lcp.hpp"
#include "offline_suffix_tree.hpp"

using namespace sdsl;
using namespace std;
using namespace stool;

namespace stool
{

uint64_t OfflineSuffixTree::isRoot(uint64_t node)
{
    return this->intervals[node].lcp == 0;
}
uint64_t OfflineSuffixTree::getRoot()
{
    return this->intervals.size() - 1;
}

uint64_t OfflineSuffixTree::getEdgeStartingPosition(uint64_t node)
{
    if (this->isRoot(node))
    {
        return UINT64_MAX;
    }
    else
    {
        uint64_t parent = this->parents[node];
        return this->sa[this->intervals[node].i] + this->intervals[parent].lcp;
    }
}
uint64_t OfflineSuffixTree::getEdgeLength(uint64_t node)
{
    if (this->isRoot(node))
    {
        return 0;
    }
    else
    {
        uint64_t parent = this->parents[node];
        return this->intervals[node].lcp - this->intervals[parent].lcp;
    }
}
char OfflineSuffixTree::getFirstChar(uint64_t node)
{
    uint64_t pos = this->getEdgeStartingPosition(node);
    if (pos == UINT64_MAX)
    {
        throw - 1;
    }
    else
    {
        return (*this->text)[pos];
    }
}
uint64_t OfflineSuffixTree::getChild(uint64_t node, char c)
{
    if (node != UINT64_MAX - 1)
    {
        for (uint64_t i = 0; i < this->childrens[node].size(); i++)
        {
            if (this->childrens[node][i].first == c)
            {
                return this->childrens[node][i].second;
            }
        }
        return UINT64_MAX;
    }
    else
    {
        return this->getRoot();
    }
}

uint64_t OfflineSuffixTree::size()
{
    return this->intervals.size();
}

void OfflineSuffixTree::constructChars()
{
    this->childrens.resize(this->size());
    uint64_t size = this->size();
    for (int64_t i = this->intervals.size() - 1; i >= 0; i--)
    {
        if (!this->isRoot(i))
        {
            uint64_t parent = this->parents[i];
            uint64_t c = this->getFirstChar(i);
            this->childrens[parent].push_back(std::pair<char, uint64_t>(c, i));
        }
        if (i % 1000000 == 0)
            std::cout << "\r"
                      << "Constructing suffix tree... : [" << (size - i) << "/" << size << "]" << std::flush;
    }
    std::cout << "[END]"<< std::endl;
    for (int64_t i = this->intervals.size() - 1; i >= 0; i--)
    {
        sort(this->childrens[i].begin(), this->childrens[i].end(), [](const std::pair<char, uint64_t> &x, const std::pair<char, uint64_t> &y) {
            return ((uint8_t)x.first) < ((uint8_t)y.first);
        });

        for (int64_t x = 1; x < this->childrens[i].size(); x++)
        {
            auto fst = this->intervals[this->childrens[i][x - 1].second].i;
            auto second = this->intervals[this->childrens[i][x].second].i;
            if (fst >= second)
            {
                std::cout << "tree error" << std::endl;
                throw - 1;
            }
        }
    }
}
uint64_t OfflineSuffixTree::computeSuffixLink(uint64_t node)
{
    if (!this->isRoot(node))
    {
        uint64_t edgeLen = this->getEdgeLength(node);
        uint64_t pos = this->getEdgeStartingPosition(node);
        uint64_t m = 0;
        uint64_t target = UINT64_MAX;
        if (this->isRoot(this->parents[node]))
        {
            target = this->getRoot();
            m += 1;
        }
        else
        {
            target = this->suffixLinks[this->parents[node]];
        }
        while (m < edgeLen)
        {
            char c = (*this->text)[pos + m];
            target = this->getChild(target, c);
            m += this->getEdgeLength(target);
        }
        return target;
    }
    else
    {
        return UINT64_MAX - 1;
    }
}
uint64_t OfflineSuffixTree::getLeaveNum(uint64_t node)
{
    //std::cout << node << std::endl;
    if (this->intervals.size() <= node)
    {
        std::cout << node << std::endl;
        throw - 1;
    }
    return this->intervals[node].j - this->intervals[node].i + 1;
}
uint64_t OfflineSuffixTree::getDegree(uint64_t node)
{
    return this->childrens[node].size();
}
uint64_t OfflineSuffixTree::selectChild(uint64_t node, uint64_t i)
{
    return this->childrens[node][i].second;
}

void OfflineSuffixTree::constructSuffixLinks()
{
    this->suffixLinks.resize(this->size(), UINT64_MAX);
    uint64_t size = this->size();

    for (int64_t i = this->intervals.size() - 1; i >= 0; i--)
    {
        this->suffixLinks[i] = this->computeSuffixLink(i);

        if (i % 1000000 == 0)
            std::cout << "\r"
                      << "Computing suffix links... : [" << (size - i) << "/" << size << "]" << std::flush;
    }
    std::cout << "[END]"<< std::endl;
}

void OfflineSuffixTree::initialize(string &_text)
{
    for (uint64_t i = 0; i < _text.size(); i++)
    {
        if (_text[i] == 0)
        {
            std::cout << "the input text contains zero." << std::endl;
            throw - 1;
        }
    }
    _text.push_back(0);
    this->text = &_text;

    vector<uint64_t> isa, lcp;
    stool::constructSA(*this->text, this->sa, isa);
    if (this->sa[0] != this->text->size() - 1)
    {
        std::cout << "the input text contains negative values." << std::endl;
        throw - 1;
    }
    constructLCP(*this->text, lcp, this->sa, isa);
    createLCPIntervals(sa, lcp, this->intervals, this->parents);

    this->constructChars();
    this->constructSuffixLinks();
}
} // namespace stool