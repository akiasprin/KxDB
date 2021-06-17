//
// Created by kevi on 2021/6/11.
//

#ifndef KXDB_AIRINMEM_HPP
#define KXDB_AIRINMEM_HPP

#include <iostream>
#include <functional>
#include <thread>
#include <future>

#include "AirinBlockData.hpp"
#include "AirinFile.hpp"
#include "../utils/RBTree.hpp"
#include "../utils/Helper.hpp"

class AirinMem {
private:
public:
    unsigned long long size;
    RBTree<AirinCell>* tree;
    future<void> flushFuture;

    AirinMem();
    ~AirinMem();

    static void flush(RBTree<AirinCell> *tree);
    static void flushCallback(AirinCell *cell, AirinFilePopulation *&pop, AirinFile *file);

    void add(AirinCell *cell);

    void close();

};


#endif //KXDB_AIRINMEM_HPP
