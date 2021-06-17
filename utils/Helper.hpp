//
// Created by kevi on 2021/6/12.
//
#ifndef KXDB_HELPER_HPP
#define KXDB_HELPER_HPP

#include <iostream>
#include <functional>
#include <thread>
#include <future>
#include <vector>

#include "Comparable.hpp"
#include "../AirinSE/AirinBlockIndex.hpp"

using namespace std;

typedef unsigned long long ULL;

class Helper {
public:

    template<typename R>
    static bool is_ready(future<R> &f) {
        return f.wait_for(chrono::seconds(0)) == future_status::ready;
    }

    static ULL getRandom(ULL const min, ULL const max) {
        return (((ULL)(unsigned int)rand() << 32) + (ULL)(unsigned int)rand()) % (max - min) + min;
    }

    static int binarySearch(vector<AirinBlockAbstract *> *v, AirinBlockIndex *k) {
        int l = 0, r = v->size(), m = 0; // r: 维持右开区间边界
        while (l < r) {
            m = (l + r) >> 1;
            AirinBlockIndex *y = dynamic_cast<AirinBlockIndex *>(v->at(m));
            if (AirinBlockIndex::lessThan(k, y)) {
                r = m;
            } else if (AirinBlockIndex::equals(k, y)) {
                return m;
            } else {
                l = m + 1;
            }
        }
        return r - 1; // 返回比他大的前一个
    }
    
    
    static int binarySearch(vector<AirinCell *> *v, AirinCell *k) {
        int l = 0, r = v->size(), m = 0; // r: 维持右开区间边界
        while (l < r) {
            m = (l + r) >> 1;
            AirinCell *y = v->at(m);
            if (k->_lessThan(y)) {
                r = m;
            } else if (k->_equals(y)) {
                return m;
            } else {
                l = m + 1;
            }
        }
        return r - 1; // 返回比他大的前一个
    }
};

#endif //KXDB_HELPER_HPP
