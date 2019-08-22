#pragma once
#include <stack>
#include <vector>
#include <unordered_set>
#include "esaxx/src/minimal_substrings/minimal_substring_iterator.hpp"

namespace stool
{
namespace lazy
{

template<typename CHAR>
std::vector<stool::LCPInterval<uint64_t>> loadOrConstructMS(std::string filename, std::vector<CHAR> &text, std::vector<uint64_t> &sa, uint64_t k_attr)
{
    std::vector<stool::LCPInterval<uint64_t>> r;
    if (filename.size() == 0)
    {
        std::vector<uint64_t> lcpArray = stool::constructLCP<CHAR, uint64_t>(text, sa);
        std::vector<CHAR> bwt = stool::constructBWT<CHAR, uint64_t>(text, sa);

        std::vector<stool::LCPInterval<uint64_t>> tmp = stool::esaxx::MinimalSubstringIterator<CHAR, uint64_t, std::vector<uint64_t>>::constructSortedMinimalSubstrings(bwt, sa, lcpArray);
        r.swap(tmp);
    }
    else
    {
        stool::load_vector<LCPInterval<uint64_t>>(filename,r, true);
    }

    if (k_attr != 0)
    {
        stool::esaxx::MinimalSubstringIterator<CHAR, uint64_t, std::vector<uint64_t>>::getKMinimalSubstrings(r, k_attr);
    }
    return r;
}


} // namespace lazy
} // namespace stool