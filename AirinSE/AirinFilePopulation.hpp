//
//  AirinFilePopulation.hpp
//  KxDB
//
//  Created by 葉朝鈞 on 2021/6/15.
//

#ifndef AirinFilePopulation_hpp
#define AirinFilePopulation_hpp

#include <stdio.h>
#include "AirinFile.hpp"
#include "AirinBlockData.hpp"
#include "AirinBlockBloomFilter.hpp"

class AirinFilePopulation {
private:
    AirinFile *f;
public:
    int size;
    AirinBlockData *blk;
    AirinBlockBloomFilter *bfblk;
    AirinFilePopulation(AirinFile *f);
    void add(AirinCell *cell);
    void close();
};

#endif /* AirinFilePopulation_hpp */
