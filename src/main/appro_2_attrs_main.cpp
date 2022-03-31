#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include "stool/include/cmdline.h"
#include "stool/include/io.hpp"
#include "stool/include/sa_bwt_lcp.hpp"
#include "../src/verification_attractor.hpp"
#include "esaxx/src/minimal_substrings/minimal_substring_iterator.hpp"
#include "../src/common.hpp"
#include "libdivsufsort/sa.hpp"
#include "../binary.hpp"
//#include "minimal_substrings.hpp"
//#include "mstree.hpp"
//#include "esaxx/src/minimal_substrings/minimal_substring_tree.hpp"

using namespace std;
using namespace sdsl;
using namespace stool;
// using namespace stool::lazy;

int main(int argc, char *argv[])
{
    // using CHAR = uint8_t;
    // using INDEX = uint64_t;

#ifdef DEBUG
    std::cout << "\033[41m";
    std::cout << "DEBUG MODE!";
    std::cout << "\e[m" << std::endl;
// std::cout << "\033[30m" << std::endl;
#endif
    cmdline::parser p;

    p.add<uint>("len", 'l', "len", true);

    p.parse_check(argc, argv);
    uint len = p.get<uint>("len");
    std::cout << len << std::endl;

    std::string enum_log_filename = "LOG_appro_2_attractors" + std::to_string(len) + ".txt";
    std::ifstream test_ifs(enum_log_filename);
    if (test_ifs.is_open())
    {
        remove(enum_log_filename);
        std::cout << "ファイルを削除しました。: " << enum_log_filename << std::endl;
    }

    std::cout << "Log file 1: " << enum_log_filename << std::endl;

    std::unordered_set<std::string> prev_sbinary_set;
    std::unordered_set<std::string> current_sbinary_set;

    std::ofstream file_out;
    file_out.open(enum_log_filename, std::ios_base::app);


    for (uint64_t i = 1; i < len; i++)
    {
        current_sbinary_set.clear();
        std::cout << "Computing all " << i << "-binary strings with 2-attractors" << std::endl;
        std::vector<std::string> binary_strings;
        if(i <= 2){
            auto tmp = stool::binary::enumerate(i);
            binary_strings.swap(tmp);
        }else{
            std::vector<std::string> tmp_vec;
            for(auto &it : prev_sbinary_set){
                tmp_vec.push_back(it);
            }
            auto new_vec = stool::binary::append_character_left_or_right(tmp_vec);
            binary_strings.swap(new_vec);
        }

        std::vector<std::string> normalized_strings = stool::binary::normalize(binary_strings);
        std::vector<bool> checker_vec;
        checker_vec.resize(normalized_strings.size());
        // std::unordered_set<std::string> binary_strings_set = stool::binary::to_set(binary_strings);

        auto start = std::chrono::system_clock::now();

        for (uint64_t i = 0; i < normalized_strings.size(); i++)
        {
            std::string binary_str = normalized_strings[i];

            bool b = stool::binary::verify_2_smallest_string_attractor(binary_str);
            checker_vec[i] = b;
            if (b)
            {
                current_sbinary_set.insert(binary_str);
            }
        }
        auto end = std::chrono::system_clock::now();
        double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        double perTime = elapsed / (double)binary_strings.size();

        std::cout << "Execution time: " << ((uint64_t)elapsed / 1000) << "[s]"
                  << "(" << perTime << "[ms] / per)" << std::endl;
        // std::cout << "Execution time: " << elapsed << "[ms]" << sd::endl;

        for (uint64_t i = 0; i < normalized_strings.size(); i++)
        {
            if (checker_vec[i])
            {
                auto binary_str = normalized_strings[i];
                auto four_strs = stool::binary::get_four_types(binary_str);
                for (auto &it : four_strs)
                {
                    current_sbinary_set.insert(it);
                }
            }
        }

        uint64_t total_count = std::pow(2, i);
        file_out << i << "-binary strings" << std::endl;
        file_out << "The number of " << i << "-binary strings: \t " << total_count << std::endl;
        file_out << "The number of tested binary strings: " << normalized_strings.size() << std::endl;
        file_out << "The number of 2-smallest attoractors (approximated count): \t" << current_sbinary_set.size() << std::endl;
        file_out << std::endl;
        file_out << "文字列 | 逆文字列 | 反転文字列 | 逆文字列の反転文字列" << std::endl;

        std::cout << i << "-binary strings" << std::endl;
        std::cout << "The number of " << i << "-binary strings: \t " << total_count << std::endl;
        std::cout << "The number of tested binary strings: " << normalized_strings.size() << std::endl;
        std::cout << "The number of 2-smallest attoractors (approximated count): \t" << current_sbinary_set.size() << std::endl;

        uint64_t ith = 0;

        for (uint64_t i = 0; i < normalized_strings.size(); i++)
        {
            if (checker_vec[i])
            {
                auto binary_str = normalized_strings[i];
                auto dp = stool::binary::get_four_types_without_duplication(binary_str);
                string message = std::to_string(ith) + ": " + dp[0] + " | " + dp[1] + " | " + dp[2] + " | " + dp[3];
                file_out << message << std::endl;


                ith += dp[0][0] != '_' ? 1 : 0;
                ith += dp[1][0] != '_' ? 1 : 0;
                ith += dp[2][0] != '_' ? 1 : 0;
                ith += dp[3][0] != '_' ? 1 : 0;
            }
        }
        file_out << "--------------------------------" << std::endl;
        file_out << std::endl;

        std::cout << "--------------------------------" << std::endl;
        std::cout << std::endl;

        /*
        for (auto &binary_str : binary_strings)
        {
        }
        */
        // std::cout << "Len: " << i << ", [" << binary_strings.size() << " / " << current_sbinary_set.size() << "]" << std::endl;

        prev_sbinary_set.swap(current_sbinary_set);
    }
    file_out.close();
    return 0;
}
