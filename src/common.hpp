#pragma once
#include <stack>
#include <vector>
#include <unordered_set>

namespace stool
{
namespace lazy
{

std::vector<stool::LCPInterval<uint64_t>> loadOrConstructMS(std::string filename, std::vector<uint8_t> &text, std::vector<uint64_t> &sa, uint64_t k_attr)
{
    std::vector<stool::LCPInterval<uint64_t>> r;
    if (filename.size() == 0)
    {

        std::vector<uint64_t> lcpArray = stool::constructLCP<uint8_t, uint64_t>(text, sa);
        std::vector<uint8_t> bwt = stool::constructBWT<uint8_t, uint64_t>(text, sa);

        std::vector<stool::LCPInterval<uint64_t>> tmp = stool::esaxx::MinimalSubstringIterator<uint8_t, uint64_t, std::vector<uint64_t>>::constructSortedMinimalSubstrings(bwt, sa, lcpArray);
        r.swap(tmp);
    }
    else
    {
        stool::load_vector<LCPInterval<uint64_t>>(filename,r);
    }

    if (k_attr != 0)
    {
        stool::esaxx::MinimalSubstringIterator<uint8_t, uint64_t, std::vector<uint64_t>>::getKMinimalSubstrings(r, k_attr);
    }
    return r;
}


} // namespace lazy
} // namespace stool