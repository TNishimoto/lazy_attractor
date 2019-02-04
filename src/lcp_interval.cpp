
#include "lcp_interval.hpp"

using namespace sdsl;
using namespace std;

namespace stool
{

LCPInterval::LCPInterval()
{
}
LCPInterval::LCPInterval(uint64_t _i, uint64_t _j, uint64_t _lcp)
{
    this->i = _i;
    this->j = _j;
    this->lcp = _lcp;
}
/*
    uint64_t minLCP(LCPInterval &prev, vector<uint64_t> &sa, vector<uint64_t> &lcp)
    {
        uint64_t lcpValue = lcp[this->i];
        uint64_t minLCP1 = this->lcp < prev.lcp ? this->lcp : prev.lcp;
        uint64_t minLCP2 = lcpValue < minLCP1 ? lcpValue : minLCP1;
        return minLCP2;
    }
    */
string LCPInterval::toString()
{
    return "[" + to_string(i) + ", " + to_string(j) + ", " + to_string(lcp) + "]";
}
bool LCPInterval::check(int_vector<> &text, vector<uint64_t> &sa)
{
    uint64_t pos = sa[this->i] + this->lcp;
    uint64_t c = pos == text.size() ? UINT64_MAX : text[pos];
    if (this->i == this->j)
        return true;
    for (uint64_t p = i; p <= j; p++)
    {
        uint64_t pos1 = sa[p] + this->lcp;
        uint64_t c1 = pos1 == text.size() ? UINT64_MAX : text[pos1];

        if (c != c1 || c1 == UINT64_MAX)
        {
            return true;
        }
    }
    return false;
}
void LCPInterval::print(int_vector<> &text, vector<uint64_t> &sa, vector<LCPInterval> &intervals)
{
    vector<string> s;
    uint64_t n = text.size();
    s.resize(n);
    for (uint64_t i = 0; i < n; i++)
    {
        s[i].resize(n, '-');
    }
    for (uint64_t i = 0; i < intervals.size(); i++)
    {
        uint64_t b = intervals[i].i;
        uint64_t e = intervals[i].j;
        char pc = '@';

        for (uint64_t x = b + 1; x <= e; x++)
        {
            uint64_t p1 = sa[x - 1];
            uint64_t p2 = sa[x];
            uint64_t y = 0;
            while (p1 + y < n && p2 + y < n)
            {
                if (text[p1 + y] == text[p2 + y])
                {
                    uint64_t c = text[p1 + y];
                    s[x - 1][y] = pc;
                    s[x][y] = pc;
                    y++;
                }
                else
                {
                    break;
                }
            }
        }
    }
    for (uint64_t i = 0; i < n; i++)
    {
        string istr = to_string(i);
        while (istr.size() < 5)
            istr += " ";
        std::cout << istr << " : " << s[i];

        uint64_t bwtC = sa[i] == 0 ? 0 : text[sa[i] - 1];
        string bwtstr = to_string(bwtC);
        while (bwtstr.size() < 5)
            bwtstr += " ";
        std::cout << bwtstr << ":";

        for (uint64_t p = sa[i]; p < text.size(); p++)
        {
            std::cout << to_string(text[p]) << ",";
        }
        std::cout << std::endl;
    }
    for (uint64_t i = 0; i < intervals.size(); i++)
    {
        std::cout << intervals[i].toString() << std::endl;
    }
}

void createLCPIntervals(vector<uint64_t> &sa, vector<uint64_t> &lcp, vector<LCPInterval> &intervals)
{
    assert(sa.size() == lcp.size());
    stack<LCPInterval> stack;
    int64_t n = sa.size();
    for (int64_t i = 0; i < n; i++)
    {
        LCPInterval fst = LCPInterval(i, i, n - sa[i]);
        while (stack.size() > 1)
        {
            LCPInterval second = stack.top();
            if (lcp[second.i] > lcp[fst.i])
            {
                stack.pop();
                uint64_t newLCP = lcp[second.i];
                uint64_t newj = second.j;
                uint64_t newi = second.i;

                while (stack.size() > 0)
                {
                    LCPInterval third = stack.top();
                    newi = third.i;
                    stack.pop();
                    if (lcp[second.i] != lcp[third.i])
                    {
                        break;
                    }
                }
                LCPInterval newInterval(newi, newj, newLCP);
                stack.push(newInterval);
                intervals.push_back(newInterval);
            }
            else
            {
                break;
            }
        }
        intervals.push_back(fst);

        stack.push(fst);
    }
    while (stack.size() > 1)
    {
        LCPInterval second = stack.top();
        stack.pop();
        uint64_t newLCP = lcp[second.i];
        uint64_t newj = second.j;
        uint64_t newi = second.i;
        while (stack.size() > 0)
        {
            LCPInterval third = stack.top();
            newi = third.i;
            stack.pop();
            if (lcp[second.i] != lcp[third.i])
            {
                break;
            }
        }
        LCPInterval newInterval(newi, newj, newLCP);
        stack.push(newInterval);
        intervals.push_back(newInterval);
    }
}
void createLCPIntervals(vector<uint64_t> &sa, vector<uint64_t> &lcp, vector<LCPInterval> &outputIntervals, vector<uint64_t> &outputParentVec)
{
    std::cout << "\r"  << "Constructing LCP Intervals..."<< std::flush;
    createLCPIntervals(sa, lcp, outputIntervals);

    outputParentVec.resize(outputIntervals.size(), UINT64_MAX);
    stack<uint64_t> stack;

    uint64_t m = outputIntervals.size();
    for (uint64_t i = 0; i < m; i++)
    {
        LCPInterval &x = outputIntervals[i];

        if (i % 1000000 == 0)
            std::cout << "\r"
                      << "Constructing LCP Intervals... : [" << i << "/" << m << "]" << std::flush;
        while (stack.size() > 0)
        {
            uint64_t top = stack.top();
            LCPInterval &y = outputIntervals[top];
            if (x.i <= y.i && y.j <= x.j)
            {
                outputParentVec[top] = i;
                stack.pop();
            }
            else
            {
                break;
            }
        }
        stack.push(i);
    }
    std::cout << "[END]" << std::endl;
}
/*
void createSuffixLinks(string &text, vector<uint64_t> &sa, vector<uint64_t> &lcp, vector<LCPInterval> &intervals, vector<uint64_t> &parentVec, vector<uint64_t> &outputSuffixLinks){
    
}
*/

} // namespace stool
