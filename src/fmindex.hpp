#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/wt_gmr.hpp>
#include <sdsl/wavelet_trees.hpp>

#include "sa_lcp.hpp"
using namespace std;
using namespace sdsl;

namespace stool
{

class FMIndex
{
  public:
    int_vector<> bwt;
    vector<uint64_t> C;
    wt_gmr<> wt;
    FMIndex(string &text, vector<uint64_t> &sa)
    {
        constructBWT(text, sa, this->bwt);
        constructC(this->bwt, this->C);
        construct_im(wt, this->bwt);
    }
    uint64_t LF(uint64_t i)
    {
        uint8_t c = this->bwt[i];
        uint64_t cNum = this->wt.rank(i, c);
        return C[c] + cNum;
    }
    uint64_t getCharCount(uint8_t c)
    {
        if (c == UINT8_MAX)
        {
            return this->bwt.size() - this->C[c];
        }
        else
        {
            return this->C[c + 1] - this->C[c];
        }
    }
    bool backwardSearch(uint64_t &b, uint64_t &e, uint8_t c)
    {
        if (e < b)
            return false;
        uint64_t charCount = this->getCharCount(c);
        uint64_t cBegNum = this->wt.rank(b, c);

        if (cBegNum == charCount)
        {
            return false;
        }
        uint64_t cBeg = this->wt.select((cBegNum + 1), c);

        uint64_t cEndNum = this->wt.rank(e + 1, c);
        if (cEndNum == 0)
        {
            return false;
        }

        uint64_t cEnd = this->wt.select(cEndNum, c);

        b = this->LF(cBeg);
        e = this->LF(cEnd);
        return true;
    }

    static void constructBWT(string &text, vector<uint64_t> &sa, int_vector<> &outputBWT)
    {
        //vector<uint64_t> sa;
        //stool::constructSA(text, sa);
        outputBWT.width(8);
        outputBWT.resize(text.size());
        for (uint64_t i = 0; i < text.size(); i++)
        {
            if (sa[i] == 0)
            {
                outputBWT[i] = text[text.size() - 1];
            }
            else
            {
                outputBWT[i] = text[sa[i] - 1];
            }
        }
    }
    static void constructC(int_vector<> &text, vector<uint64_t> &output)
    {
        vector<uint64_t> tmp;
        tmp.resize(UINT8_MAX, 0);
        output.resize(UINT8_MAX, 0);

        for (uint64_t i = 0; i < text.size(); i++)
        {
            tmp[text[i]]++;
        }
        for (uint64_t i = 1; i <= 255; i++)
        {
            output[i] = output[i - 1] + tmp[i - 1];
        }
    }
};

} // namespace stool