//
//  AirinFilePopulation.cpp
//  KxDB
//
//  Created by 葉朝鈞 on 2021/6/15.
//

#include "AirinFilePopulation.hpp"

AirinFilePopulation::AirinFilePopulation(AirinFile *_f) {
    f = _f;
    size = 0;
    blk = new AirinBlockData();
    bfblk = new AirinBlockBloomFilter();
}

void AirinFilePopulation::add(AirinCell *cell) {
    blk->put(cell);
    bfblk->put(cell);
    size += cell->size();
    if (size >= DATA_BLOCK_SIZE_BYTES) {
        f->putBLK(blk);
        blk->reset();
        size = 0;
    }
    if (bfblk->n >= BLOOM_FILTER_ELEMENT_NUM) {
        f->putBFBLK(bfblk);
        bfblk->reset();
    }
}

void AirinFilePopulation::close() {
    if (blk->n > 0) {
        f->putBLK(blk);
        blk->reset();
        size = 0;
    }
    if (bfblk->n > 0) {
        f->putBFBLK(bfblk);
        bfblk->reset();
    }
    delete blk;
    delete bfblk;
}
