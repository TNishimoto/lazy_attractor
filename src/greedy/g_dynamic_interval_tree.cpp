#include "g_dynamic_interval_tree.hpp"

namespace stool{
    namespace lazy{
        GDynamicIntervalTree::GDynamicIntervalTree(std::vector<uint64_t> &_sa, std::vector<uint64_t> &_isa, std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents, uint64_t textSize) : tree(_intervals, _parents, textSize), sa(_sa), isa(_isa), intervals(_intervals)
    {
    }
    void GDynamicIntervalTree::construct()
    {
        bool isPrint = sa.size() > 1000;
        if(isPrint)std::cout << "Constructing GDynamicIntervalTree..." << std::endl;
        std::vector<uint64_t> posVec;
        posVec.resize(this->sa.size());
        for (uint64_t i = 0; i < sa.size(); i++)
        {
            posVec[i] = i;
        }
        if(isPrint)std::cout << "Sorting positions on the text..." << std::flush;
        std::sort(posVec.begin(), posVec.end(), [&](const uint64_t &x, const uint64_t &y) {
            uint64_t x_lcp = intervals[this->tree.getLowestLCPIntervalID(x)].lcp;
            uint64_t y_lcp = intervals[this->tree.getLowestLCPIntervalID(y)].lcp;
            if (x_lcp != y_lcp)
            {
                return x_lcp > y_lcp;
            }
            else
            {
                return x < y;
            }
        });
        if(isPrint)std::cout << "[END]" << std::endl;


        if(isPrint)std::cout << "Computing the distance vector..." << std::flush;
        this->distanceVec.resize(sa.size(), 0);
        std::vector<uint64_t> idVec;
        idVec.resize(sa.size(), UINT64_MAX);
        stool::Counter counter;

        for (uint64_t i = 0; i < posVec.size(); i++)
        {
            if(isPrint)counter.increment();
            uint64_t x = posVec[i];
            uint64_t id = this->tree.getLowestLCPIntervalID(x);
            uint64_t x_lcp = intervals[id].lcp;
            uint64_t distance = 1;
            while (x_lcp > 0)
            {
                uint64_t diff = 1;
                if (idVec[x] != UINT64_MAX && distance <= this->distanceVec[x])
                {
                    uint64_t prev_x_lcp = this->intervals[idVec[x]].lcp - this->distanceVec[x] + 1;
                    if (prev_x_lcp >= x_lcp)
                    {
                        break;
                    }
                    else
                    {
                        // ayasii
                        //diff = prev_x_lcp;
                    }
                }
                else
                {
                    this->distanceVec[x] = distance;
                    idVec[x] = id;
                }

                if (sa[x] + diff == sa.size())
                {
                    break;
                }
                else
                {
                    x = isa[sa[x] + diff];
                    x_lcp -= diff;
                    distance += diff;
                }
            }
        }
        if(isPrint)std::cout << "[END]" << std::endl;

        //assert(false);
        /*
        this->msVec.resize(intervals.size());
        for (uint64_t i = 0; i < idVec.size(); i++)
        {
            this->msVec[idVec[i]].push_back(i);
        }
        std::cout << "Constructing GDynamicIntervalTree[END]" << std::endl;
        */
    }

    std::vector<uint64_t> GDynamicIntervalTree::getAndRemoveCapturedLCPIntervals(uint64_t sa_index)
    {
        std::vector<uint64_t> r;
        int64_t distance = this->distanceVec[sa_index];
        for (int64_t i = 0; i < distance; i++)
        {
            while (true)
            {
                uint64_t id = this->tree.getLowestLCPIntervalID(sa_index);
                if ((int64_t)intervals[id].lcp > i)
                {
                    r.push_back(id);
                    //std::cout << "add id " << id << std::endl;
                    //assert(!this->tree.checkRemovedInterval(id));
                    this->tree.removeLowestLCPInterval(sa_index);
                    //assert(this->tree.checkRemovedInterval(id));
                }
                else
                {
                    break;
                }
            }
            if (i + 1 < distance)
            {
                sa_index = isa[sa[sa_index] - 1];
            }
        }
        /*
        for (uint64_t x = 0; x < r.size(); x++)
        {
            this->update(r[x]);
        }
        */

        std::sort(r.begin(), r.end());
        #ifdef DEBUG
        for (uint64_t i = 1; i < r.size(); i++)
        {
            assert(r[i] != r[i - 1]);
        }
        #endif
        return r;
    }
    void GDynamicIntervalTree::debug(std::unordered_set<uint64_t> &currentIntervals, std::vector<LCPInterval<uint64_t>> &intervals, uint64_t textSize)
    {
        std::vector<uint64_t> r1 = stool::lazy::LowestNodeQuery::constructLowestNodeIDVec(currentIntervals, intervals, textSize);
        std::vector<uint64_t> r2 = this->tree.constructLowestNodeIDVec(textSize);

        for (uint64_t i = 0; i < r1.size(); i++)
        {
            if (r1[i] == UINT64_MAX)
            {
                r1[i] = 0;
            }
        }

        assert(r1.size() == r2.size());
        /*
        for(uint64_t i=0;i<r1.size();i++){
            std::cout << r1[i] << ", ";
        }
        std::cout << std::endl;

        for(uint64_t i=0;i<r2.size();i++){
            std::cout << r2[i] << ", ";
        }
        std::cout << std::endl;
        */

        for (uint64_t i = 0; i < r1.size(); i++)
        {
            assert(r1[i] == r2[i]);
        }
    }
    std::vector<uint64_t> GDynamicIntervalTree::removeCapturedIntervals(uint64_t attractor, std::unordered_set<uint64_t> &currentIntervals, std::vector<LCPInterval<uint64_t>> &intervals, std::vector<uint64_t> &sa)
{
    std::vector<uint64_t> r;
    for (auto &it : currentIntervals)
    {
        LCPInterval<uint64_t> &interval = intervals[it];
        for (uint64_t x = interval.i; x <= interval.j; x++)
        {
            if (sa[x] <= attractor && attractor <= sa[x] + interval.lcp - 1)
            {
                r.push_back(it);
                break;
            }
        }
    }
    for (auto &it : r)
    {
        currentIntervals.erase(it);
    }
    return r;
}
    }
}