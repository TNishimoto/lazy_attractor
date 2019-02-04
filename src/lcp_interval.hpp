#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include <sdsl/suffix_arrays.hpp>
#include <sdsl/suffix_trees.hpp>

using namespace sdsl;
using namespace std;



namespace stool
{

class LCPInterval
{
  public:
    uint64_t i;
    uint64_t j;
    uint64_t lcp;
    LCPInterval();
    LCPInterval(uint64_t _i, uint64_t _j, uint64_t _lcp);
    string toString();
    bool check(int_vector<> &text, vector<uint64_t> &sa);
    static void print(int_vector<> &text, vector<uint64_t> &sa, vector<LCPInterval> &intervals);

    bool operator<( const LCPInterval& right ) const {
        if(this->i == right.i){
            if(this->j == right.j){
                return this->lcp < right.lcp;
            }else{
                return this->j < right.j;
            }
        }else{
            return this->i < right.i;
        }
    }
    bool operator==(const LCPInterval& rhs) const {
        const LCPInterval& lhs = *this;
        bool b = lhs.i == rhs.i && lhs.j == rhs.j && lhs.lcp == rhs.lcp;
        //if(!b) std::cout << "b" << std::endl;
        return b;
    }
    /*
    bool operator!=(const LCPInterval& rhs) const {
    return !(this->operator==(rhs));
    }
    */
    bool containsPosition(vector<uint64_t> &sa, uint64_t pos) const {
        for(uint64_t x=this->i;x<=this->j;x++){
            if(sa[x] <= pos && pos <= sa[x]+this->lcp-1){
                return true;
            }
        }
        return false;
    }
};
void createLCPIntervals(vector<uint64_t> &sa, vector<uint64_t> &lcp, vector<LCPInterval> &intervals);
void createLCPIntervals(vector<uint64_t> &sa, vector<uint64_t> &lcp, vector<LCPInterval> &outputIntervals, vector<uint64_t> &outputParentVec);

//void createSuffixLinks(string &text, vector<uint64_t> &sa, vector<uint64_t> &lcp, vector<LCPInterval> &intervals, vector<uint64_t> &parentVec, vector<uint64_t> &outputSuffixLinks);

}

namespace std
{
	template <>
	struct hash<stool::LCPInterval>
	{
		std::size_t operator () ( stool::LCPInterval const &key ) const
		{
			return ((1234567891234ull) * key.i) ^ ((789000001234ull) * key.j) ^ key.lcp;
		}
	};
}