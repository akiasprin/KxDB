//
// Created by kevi on 2021/6/13.
//

#include "AirinFileScanner.hpp"

AirinFileScanner::AirinFileScanner(AirinStore *_store, AirinFile *_f) {
    f = _f;
    store = _store;
    i = offset = cSizeHint = 0;
    oldOffset = -1;
    blk = NULL;
    currentCell = NULL;
}

void AirinFileScanner::seek() {
    store->getBLK(f, offset, (AirinBlockAbstract *&) blk, cSizeHint);
    currentCell = blk->cells->at(0);
}

void AirinFileScanner::seek(AirinCell *keywordCell) {
    int p = 0;

    AirinBlockIndex *keywordIndexBlock = new AirinBlockIndex(1, 1, new vector<AirinCell *>{new AirinCell(keywordCell)});
    p = Helper::binarySearch(f->idxOnRead, keywordIndexBlock);
    delete keywordIndexBlock;
    if (p == -1) p = 0;
    AirinBlockIndex *rootIdxBlk = dynamic_cast<AirinBlockIndex *>(f->idxOnRead->at(p));

    p = Helper::binarySearch(rootIdxBlk->cells, keywordCell);
    if (p == -1) p = 0;
    AirinCell *cell = rootIdxBlk->cells->at(p);

    unsigned long long oldTmpOffset = f->fsize;
    AirinBlockData *tmpBlk = nullptr;
    while (true) {
        AirinBlockIndex::getOffsetFromIndexCell(cell, offset);
        store->getBLK(f, offset, (AirinBlockAbstract *&) tmpBlk, cSizeHint);
        p = Helper::binarySearch(tmpBlk->cells, keywordCell);
        if (tmpBlk->id > 0) {
            // 二分返回不比当前大的前一个位置，即返回等于或小于该值的位置
            // 索引段是 [0, 1), [1, 2), [2, 3) （数组点记录区间最小值）
            // 当查询 0.5 时候，二分返回 [0, 1)，符合预期
            if (p == -1) p = 0;
            if (oldTmpOffset != f->fsize) {
                store->release(f, oldTmpOffset);
            }
            oldTmpOffset = offset;
            cell = tmpBlk->cells->at(p);
        } else {
            // 二分返回不比当前大的前一个位置，即返回等于或小于该值的位置
            // 数据段也是 0, 1, 2, 3
            // 由于存在时间戳，查询是 Key 的前一个，相当于 0.9 查询 1（因为查询是 K1, 时间戳是 -1，数据段记录是 K1，时间戳是非负数）
            // 当查询 0.9 时候，需要返回 1，即二分需要返回大于该值的位置，那么推延一位即可
            // 但是当查询就是 1 时候，就不需要推延了
            blk = tmpBlk;
            i = p;
            if (i == -1) i = 0;
            if (AirinCell::lessThan(blk->cells->at(i), keywordCell)) {
                next();
            } else {
                currentCell = blk->cells->at(i);
            }
            break;
        }
    }
}

AirinCell* AirinFileScanner::next() {
    if (oldOffset != offset) {
        if (oldOffset != -1) {
             store->release(f, oldOffset);
        }
        oldOffset = offset;
    }
    i++;
    if (i >= blk->n) {
        blk = nullptr;
        if (!store->getRearDataBLK(f, offset, blk, cSizeHint)) {
            i = -1;
        } else {
            i = 0;
        }
    }
    if (i == -1) {
        return nullptr;
    }
    currentCell = blk->cells->at(i);
    return currentCell;
}

bool AirinFileScanner::contains(AirinCell *keywordCell) {
    int p = 0;

    AirinBlockIndex *keywordIndexBlock = new AirinBlockIndex(1, 1, new vector<AirinCell *>{new AirinCell(keywordCell)});
    p = Helper::binarySearch(f->idxOnReadBF, keywordIndexBlock);
    delete keywordIndexBlock;
    if (p == -1) p = 0;
    AirinBlockIndex *rootIdxBlk = dynamic_cast<AirinBlockIndex *>(f->idxOnReadBF->at(p));

    p = Helper::binarySearch(rootIdxBlk->cells, keywordCell);
    if (p == -1) p = 0;
    AirinCell *cell = rootIdxBlk->cells->at(p);

    int cSizeHintBF = 0;
    unsigned long long oldTmpOffset = f->fsize, tmpOffset;
    AirinBlockAbstract *tmpBlk = nullptr;
    while (true) {
        AirinBlockIndex::getOffsetFromIndexCell(cell, tmpOffset);
        store->getBLK(f, tmpOffset, (AirinBlockAbstract *&) tmpBlk, cSizeHintBF);
        if (tmpBlk->type() > 0) {
            AirinBlockData *tmpBlkData = dynamic_cast<AirinBlockData *>(tmpBlk);
            p = Helper::binarySearch(tmpBlkData->cells, keywordCell);
            if (p == -1) p = 0;
            if (oldTmpOffset != f->fsize) {
                store->release(f, oldTmpOffset);
            }
            oldTmpOffset = tmpOffset;
            cell = tmpBlkData->cells->at(p);
        } else {
            AirinBlockBloomFilter *tmpBlkBF = dynamic_cast<AirinBlockBloomFilter *>(tmpBlk);
            return tmpBlkBF->contains(keywordCell);
        }
    }
}

bool AirinFileScanner::_lessThan(AirinFileScanner *rhs) {
    return AirinCell::lessThan(currentCell, rhs->currentCell);
}

bool AirinFileScanner::_equals(AirinFileScanner *rhs) {
    return AirinCell::equals(currentCell, rhs->currentCell);
}

bool AirinFileScanner::lessThan(AirinFileScanner *lhs, AirinFileScanner *rhs) {
    return AirinCell::lessThan(lhs->currentCell, rhs->currentCell);
}

bool AirinFileScanner::equals(AirinFileScanner *lhs, AirinFileScanner *rhs) {
    return AirinCell::equals(lhs->currentCell, rhs->currentCell);
}

