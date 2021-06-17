//
//  AirinBlockAbstract.cpp
//  KxDB
//
//  Created by 葉朝鈞 on 2021/6/16.
//

#include "AirinBlockAbstract.hpp"
#include "AirinBlockBloomFilter.hpp"
#include "AirinBlockIndex.hpp"
#include "AirinCell.hpp"

int8_t AirinBlockAbstract::getTypeId(unsigned char *buf) {
    int8_t typeId;
    memcpy(&typeId, buf, sizeof typeId);
    return typeId;
}

int AirinBlockAbstract::read(unsigned char *buf, AirinBlockAbstract *&blk) {
    int8_t typeId = getTypeId(buf);
    if (typeId == BLOCK_TYPE_BLOOM_FILTER) {
        blk = new AirinBlockBloomFilter();
        return blk->readBFBLK(buf);
    } else if (typeId == BLOCK_TYPE_DATA) {
        blk = new AirinBlockData();
        return blk->readBLK(buf);
    } else {
        blk = new AirinBlockIndex(typeId);
        return blk->readBLK(buf);
    }
}

void AirinBlockAbstract::write(unsigned char *buf) {
    int8_t typeId = type();
    if (typeId == BLOCK_TYPE_BLOOM_FILTER) {
        return writeBFBLK(buf);
    } else {
        return writeBLK(buf);
    }
}
