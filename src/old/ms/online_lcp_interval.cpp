#include "online_lcp_interval.hpp"

using namespace sdsl;
using namespace std;

namespace stool{


    OnlineLCPInterval::OnlineLCPInterval(vector<uint64_t> &_sa, vector<uint64_t> &_lcp) : sa(_sa), lcp(_lcp)
    {
        n = sa.size();
    }
bool OnlineLCPInterval::takeFront(SpecializedLCPInterval &outputInterval)
  {
    while (true)
    {
      if (this->outputQueue.size() > 0)
      {
        SpecializedLCPInterval sli = this->outputQueue.front();
        outputInterval.i = sli.i;
        outputInterval.j = sli.j;
        outputInterval.parentEdgeLength = sli.parentEdgeLength;
        this->outputQueue.pop();
        return true;
      }
      else
      {
        if(!this->next()){
          return false;
        }
      }
    }
  }
    bool OnlineLCPInterval::next()
    {

        while (true)
        {
            bool b1 = this->nextLCPInterval();
            bool b2 = this->nextLCPIntervalWithParent();

            if (!b1 && !b2)
            {
                if(stack2.size() > 0){
                std::pair<uint64_t, uint64_t> top = stack2.top();
                outputQueue.push(SpecializedLCPInterval(top.first, top.second, UINT64_MAX));
                stack2.pop();
                return true;
                }else{
                return false;
                }

            }
            else if (b2)
            {
                return true;
            }
        }
        return false;
    }
    bool OnlineLCPInterval::nextLCPIntervalWithParent()
    {
        while (queue.size() > 0)
        {
            LCPInterval x = queue.front();
            queue.pop();
            intervalCount++;

            while (stack2.size() > 0)
            {
                std::pair<uint64_t, uint64_t> top = stack2.top();
                if (x.i <= top.first && top.second <= x.j)
                {
                    outputQueue.push(SpecializedLCPInterval(top.first, top.second, x.lcp));
                    stack2.pop();
                }
                else
                {
                    break;
                }
            }
            stack2.push(std::pair<uint64_t, uint64_t>(x.i, x.j));
        }
        return outputQueue.size() > 0;
    }

    bool OnlineLCPInterval::nextLCPInterval()
    {
        //std::cout << "next/" << counter_i << "/" << n << std::endl;
        while (counter_i < n)
        {
            LCPInterval fst = LCPInterval(counter_i, counter_i, n - sa[counter_i]);
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
                    this->queue.push(newInterval);
                    //std::cout << "push: " << newInterval.toString() << std::endl;
                    stack.push(newInterval);
                }
                else
                {
                    break;
                }
            }
            this->queue.push(fst);
            stack.push(fst);
            //std::cout << "push: " << fst.toString() << std::endl;

            counter_i++;

            return true;
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
            this->queue.push(newInterval);
            return true;
        }
        return false;
    }

    void OnlineLCPInterval::testcompute(string &text, vector<LCPInterval> &outputIntervals)
    {
        for (uint64_t i = 0; i < text.size(); i++)
        {
            if (text[i] == 0)
            {
                std::cout << "the input text contains zero." << std::endl;
                throw - 1;
            }
        }
        text.push_back(0);

        vector<uint64_t> sa, isa, lcp, parents;
        stool::constructSA(text, sa, isa);
        if (sa[0] != text.size() - 1)
        {
            std::cout << "the input text contains negative values." << std::endl;
            throw - 1;
        }
        constructLCP(text, lcp, sa, isa);

        OnlineLCPInterval oli = OnlineLCPInterval(sa, lcp);
        while (true)
        {
            if (oli.next())
            {
                while (oli.outputQueue.size() > 0)
                {
                    SpecializedLCPInterval sli = oli.outputQueue.front();
                    outputIntervals.push_back(LCPInterval(sli.i, sli.j, 0));
                    oli.outputQueue.pop();
                }
            }
            else
            {
                break;
            }
        }
        text.pop_back();
    }
}