#pragma once
#include <stack>
#include <vector>
#include <string>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include "stool/src/cmdline.h"
#include "stool/src/io.h"
#include "stool/src/sa_bwt_lcp.hpp"
#include "./verification_attractor.hpp"
#include "esaxx/src/minimal_substrings/minimal_substring_iterator.hpp"
#include "libdivsufsort/sa.hpp"

namespace stool
{
    namespace binary
    {
        /*
        std::string translate_to_string(std::vector<bool> &str)
        {
            std::string p;
            for (uint64_t i = 0; i < str.size(); i++)
            {
                p.push_back(str[i] ? 'a' : 'b');
            }
            return p;
        }
        */
        std::string copy(const std::string &item)
        {
            return item.substr(0, item.size());
            /*
            std::vector<bool> r;
            for (uint64_t i = 0; i < item.size(); i++)
            {
                r.push_back(item[i]);
            }
            return r;
            */
        }

        std::vector<std::string> enumerate(const std::vector<std::string> &strs)
        {
            std::vector<std::string> r;
            for (auto &it : strs)
            {
                auto strA = copy(it);
                strA.push_back('a');
                r.push_back(strA);

                auto strB = copy(it);
                strB.push_back('b');
                r.push_back(strB);
            }
            return r;
        }
        std::vector<std::string> append_character_left_or_right(const std::vector<std::string> &strs){
            std::unordered_set<std::string> set;
            for (auto &it : strs)
            {
                auto strA = copy(it);
                strA.push_back('a');
                set.insert(strA);

                auto strB = copy(it);
                strB.push_back('b');
                set.insert(strB);

                std::string Astr = "a";
                Astr.append(it);
                set.insert(Astr);

                std::string Bstr = "b";
                Bstr.append(it);
                set.insert(Bstr);
            }
            std::vector<std::string> r;
            for(auto &it : set){
                r.push_back(it);
            }
            //std::sort(r.begin(), r.end());
            return r;

        }

        std::vector<std::string> enumerate(uint64_t len)
        {
            std::vector<std::string> tmp;
            std::string empty = "";
            tmp.push_back(empty);
            for (uint64_t i = 0; i < len; i++)
            {
                tmp = enumerate(tmp);
            }
            return tmp;
        }

        std::unordered_set<std::string> to_set(const std::vector<std::string> &binary_strings)
        {
            std::unordered_set<std::string> r;
            for (auto &it : binary_strings)
            {
                r.insert(it);
            }
            return r;
        }
        std::string to_reversed_string(const std::string &str)
        {
            std::string s;
            s.resize(str.size());
            for (uint64_t i = 0; i < str.size(); i++)
            {
                s[s.size() - 1 - i] = str[i];
            }
            return s;
        }
        std::string to_inverted_string(const std::string &str)
        {
            std::string s;
            s.resize(str.size());
            for (uint64_t i = 0; i < str.size(); i++)
            {
                s[i] = str[i] == 'a' ? 'b' : 'a';
            }
            return s;
        }

        std::vector<uint8_t> translate(const std::string &str)
        {
            std::vector<uint8_t> r;
            for (uint64_t i = 0; i < str.size(); i++)
            {
                r.push_back(str[i]);
            }
            return r;
        }

        void get_indexes(const std::string &str, std::vector<uint64_t> &a_indexes, std::vector<uint64_t> &b_indexes)
        {
            a_indexes.clear();
            b_indexes.clear();
            for (uint64_t i = 0; i < str.size(); i++)
            {
                if (str[i] == 'a')
                {
                    a_indexes.push_back(i);
                }
                else
                {
                    b_indexes.push_back(i);
                }
            }
        }
        /*
        void print_string(std::vector<uint8_t> &str)
        {
            std::string s;
            for (auto c : str)
            {
                s.push_back(c);
            }
            std::cout << s << std::endl;
        }
        */

        std::vector<std::string> get_four_types(const std::string &str)
        {
            auto rev_str = stool::binary::to_reversed_string(str);
            auto inv_str = stool::binary::to_inverted_string(str);
            auto inv_rev_str = stool::binary::to_inverted_string(rev_str);

            return std::vector<std::string>{str, rev_str, inv_str, inv_rev_str};
        }
        std::vector<std::string> get_four_types_without_duplication(const std::string &str)
        {
            auto vec = get_four_types(str);
            for (uint64_t i = 0; i < vec.size(); i++)
            {
                for (uint64_t j = i + 1; j < vec.size(); j++)
                {
                    if (vec[i] == vec[j])
                    {
                        for (uint64_t x = 0; x < vec[j].size(); x++)
                        {
                            vec[j][x] = '_';
                        }
                    }
                }
            }
            return vec;
        }

        std::string get_normalized_form(const std::string &str)
        {
            auto vec = get_four_types(str);
            std::sort(vec.begin(), vec.end());
            return vec[0];
        }
        std::vector<std::string> normalize(const std::vector<std::string> &strings)
        {
            std::unordered_set<std::string> set;
            for (auto &it : strings)
            {
                auto nstr = get_normalized_form(it);
                set.insert(nstr);
            }
            std::vector<std::string> r;
            for (auto &it : set)
            {
                r.push_back(it);
            }
            std::sort(r.begin(), r.end());
            return r;
        }

        bool verify_2_smallest_string_attractor(const std::string &str)
        {
            std::vector<uint64_t> a_indexes, b_indexes;
            get_indexes(str, a_indexes, b_indexes);
            if (a_indexes.size() == 0 || b_indexes.size() == 0)
            {
                return false;
            }

            std::vector<uint8_t> text = translate(str);
            text.push_back(0);
            std::vector<uint64_t> sa = stool::construct_suffix_array(text);
            std::vector<stool::LCPInterval<uint64_t>> minimalSubstrings = stool::lazy::loadOrConstructMS("", text, sa, 0);
            std::vector<uint64_t> isa = stool::constructISA(text, sa);

            std::vector<uint64_t> attractors;
            for (uint64_t i = 0; i < a_indexes.size(); i++)
            {
                for (uint64_t j = 0; j < b_indexes.size(); j++)
                {
                    attractors.clear();
                    if (i < j)
                    {
                        attractors.push_back(a_indexes[i]);
                        attractors.push_back(b_indexes[j]);
                    }
                    else
                    {
                        attractors.push_back(b_indexes[j]);
                        attractors.push_back(a_indexes[i]);
                    }
                    attractors.push_back(str.size());
                    std::sort(attractors.begin(), attractors.end());
                    std::vector<uint64_t> freeIntervalIndexes = lazy::VerificationAttractor::getFreeIntervals(sa, isa, minimalSubstrings, attractors);

                    if (freeIntervalIndexes.size() == 0)
                    {
                        return true;
                    }
                }
            }
            return false;
        }

    }
}