//
// Created by kevi on 2021/6/14.
//

#include <iostream>
#include <vector>

#ifndef KXDB_HEAP_HPP
#define KXDB_HEAP_HPP

using namespace std;

template<class T>
class Heap {
public:

    int tail;
    vector<T *> *vec;

    Heap() {
        vec = new vector<T *>;
        vec->resize(8);
        tail = 1;
    }
    
    ~Heap() {
        delete vec;
    }

    bool empty() {
        return tail <= 1;
    }

    void push(T *e) {
        if (tail >= vec->capacity()) {
            vec->resize(vec->capacity() << 1);
        }
        vec->at(tail) = e;
        int i = tail;
        while (i > 1) {
            int p = i >> 1;
            if (vec->at(i)->_lessThan(vec->at(p))) {
                swap(vec->at(i), vec->at(p));
            } else {
                break;
            }
            i = p;
        }
        tail++;
    }

    void pop(T *&e) {
        e = vec->at(1);
        vec->at(1) = vec->at(--tail);
        int i = 1;
        while (true) {
            int l = i << 1, r = (i << 1) | 1, m = 0;
            if (l >= tail) {
                break;
            }  else if (r < tail && vec->at(r)->_lessThan(vec->at(l))) {
                m = r;
            } else {
                m = l;
            }
            if (vec->at(m)->_lessThan(vec->at(i))) {
                swap(vec->at(m), vec->at(i));
            } else {
                break;
            }
            i = m;
        }
    }
};

#endif //KXDB_HEAP_HPP
