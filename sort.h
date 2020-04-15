#pragma once
#include <algorithm>
#include <thread>
#include <future>

namespace kth
{

    template <typename Iter>
    void sort(Iter begin, Iter end, unsigned int N = std::thread::hardware_concurrency() / 2)
    {
        auto length = end - begin;

        if (length < 1024 || N < 2)
        {
            std::sort(begin, end);
            return;
        }

        Iter middle = begin + (length / 2);

        auto left_f = std::async(std::launch::async, sort<Iter>, begin, middle, N - 2);
        sort(middle, end, N - 2);
        left_f.wait();

        std::inplace_merge(begin, middle, end);
    }

}