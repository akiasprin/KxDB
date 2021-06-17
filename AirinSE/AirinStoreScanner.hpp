//
// Created by kevi on 2021/6/13.
//

#ifndef KXDB_AIRINSTORESCANNER_HPP
#define KXDB_AIRINSTORESCANNER_HPP

#include "AirinFile.hpp"
#include "AirinFileScanner.hpp"
#include "../utils/Heap.hpp"

using namespace std;

class AirinStoreScanner {
public:

    AirinStore *store;
    Heap<AirinFileScanner> *heap;

    AirinCell *from, *to;
    bool useBloomFilter;

    AirinStoreScanner(AirinStore *_store);

    void init();
    void init(AirinCell *_from, AirinCell *_to, bool _useBloomFilter = false);
    AirinCell* next();
    
    void reset();
    void close();
};


#endif //KXDB_AIRINSTORESCANNER_HPP
