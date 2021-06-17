//
//  AirinFile.cpp
//  KxDB
//
//  Created by 葉朝鈞 on 2021/6/4.
//

#include "AirinFile.hpp"

using namespace std;

AirinFile::AirinFile(char *_filename) {
    filename = _filename;
    idxOnWriteL1 = new vector<AirinBlockIndex *>();
    idxOnWriteL2 = new vector<AirinBlockIndex *>();
    idxOnWriteL3 = new vector<AirinBlockIndex *>();
    idxSizeOnWriteL1 = 0;
    idxSizeOnWriteL2 = 0;
    idxSizeOnWriteL3 = 0;
    idxOnWriteBFL1 = new vector<AirinBlockIndex *>();
    idxOnWriteBFL2 = new vector<AirinBlockIndex *>();
    idxSizeOnWriteBFL1 = 0;
    idxSizeOnWriteBFL2 = 0;
    idxOnRead = new vector<AirinBlockAbstract *>();
    idxOnReadBF = new vector<AirinBlockAbstract *>();
    curIdxBlk = nullptr;
    curIdxBFBlk = nullptr;
}

AirinFile::~AirinFile() {
    for (int i = 0; i < idxOnWriteL1->size(); i++) {
        delete idxOnWriteL1->at(i);
    }
    for (int i = 0; i < idxOnWriteL2->size(); i++) {
        delete idxOnWriteL2->at(i);
    }
    for (int i = 0; i < idxOnWriteL3->size(); i++) {
        delete idxOnWriteL3->at(i);
    }
    for (int i = 0; i < idxOnRead->size(); i++) {
        delete idxOnRead->at(i);
    }
    delete idxOnWriteL1;
    delete idxOnWriteL2;
    delete idxOnWriteL3;
    delete idxOnRead;
    delete curIdxBlk;
}

void AirinFile::getBLK(unsigned long long &offset, AirinBlockAbstract *&block, int &cSizeHint) {
    int cSize = 0, dSize = 0, sizeofSize = sizeof cSize + sizeof dSize;
    unsigned char buf[sizeofSize];
    pread(fd, buf, sizeofSize, offset);
    memcpy(&dSize, buf, sizeof dSize);
    memcpy(&cSize, buf + sizeof dSize, sizeof cSize);
    unsigned char blkBuf[dSize];
    unsigned char decompressTmp[cSize];

    pread(fd, decompressTmp, cSize, offset + sizeofSize);
    {
        size_t const _dSize = ZSTD_decompress(blkBuf, dSize, decompressTmp, cSize);
    }
    AirinBlockAbstract::read(blkBuf, block);
    cSizeHint = cSize;
}

int AirinFile::writeBLK(AirinBlockAbstract *block) {
    int dSize = block->size(), cSize = 0, _cSize = ZSTD_compressBound(dSize);

    unsigned char *tmp = new unsigned char[dSize];

    block->write(tmp);

    int sizeofSize = sizeof dSize + sizeof cSize;
    unsigned char *compressTmp = new unsigned char[_cSize + sizeofSize];

    {
        cSize = ZSTD_compress(compressTmp + sizeofSize, _cSize, tmp, dSize, 1);
    }

    memcpy(compressTmp, &dSize, sizeof dSize);
    memcpy(compressTmp + sizeof dSize, &cSize, sizeof cSize);

    fwrite(compressTmp, sizeofSize + cSize, 1, fp);
    int incr = sizeofSize + cSize;
    fsize += incr;
    
    delete[] tmp;
    delete[] compressTmp;
    return incr;
}

void AirinFile::putBLK(AirinBlockData *block) {
    if (curIdxBlk == nullptr) {
        curIdxBlk = new AirinBlockIndex(1);
    }

    unsigned char offsetBuf[sizeof fsize];
    memcpy(offsetBuf, &fsize, sizeof fsize);
    AirinCell *idxCell = new AirinCell(block->cells->at(0)->key, offsetBuf, block->cells->at(0)->klen, sizeof fsize);

    writeBLK(block);

    curIdxBlk->put(idxCell);

    if (curIdxBlk->size() >= INDEX_BLOCK_SIZE_BYTES) {
        idxOnWriteL1->push_back(curIdxBlk);
        idxSizeOnWriteL1 += curIdxBlk->size(); // 块级别
        curIdxBlk = new AirinBlockIndex(1);
    }

    if (idxSizeOnWriteL1 >= FIRST_IDX_LEVEL_INMEMORY_SIZE_BYTES) {
//        cout << "Level 1 Index Block Flushing." << " ( " << endl;
//        cout << "  ";
//        idxOnWriteL1->front()->cells->front()->debug();
//        cout << "  ";
//        idxOnWriteL1->back()->cells->back()->debug();
//        cout << ") size = " << idxSizeOnWriteL1 << endl;
        flushL1();
    }

    if (idxSizeOnWriteL2 >= SECOND_IDX_LEVEL_INMEMORY_SIZE_BYTES) {
        cout << "Level 2 Index Block Flushing." << " ( " << endl;
        cout << "  ";
        idxOnWriteL2->front()->cells->front()->debug();
        cout << "  ";
        idxOnWriteL2->back()->cells->back()->debug();
        cout << ") size = " << idxSizeOnWriteL2 << endl;
        flushL2();
    }
}

void AirinFile::putBFBLK(AirinBlockBloomFilter *block) {
    if (curIdxBFBlk == nullptr) {
        curIdxBFBlk = new AirinBlockIndex(1);
    }
    unsigned char offsetBuf[sizeof fsize];
    memcpy(offsetBuf, &fsize, sizeof fsize);
    AirinCell *idxCell = new AirinCell(block->head->key, offsetBuf, block->head->klen, sizeof fsize);

    writeBLK(block);

    curIdxBFBlk->put(idxCell);

    if (curIdxBFBlk->size() >= INDEX_BLOCK_SIZE_BYTES) {
        idxOnWriteBFL1->push_back(curIdxBFBlk);
        idxSizeOnWriteBFL1 += curIdxBFBlk->size(); // 块级别
        curIdxBFBlk = new AirinBlockIndex(1);
    }

    if (idxSizeOnWriteBFL1 >= FIRST_IDX_LEVEL_INMEMORY_SIZE_BYTES) {
        flushBFL1();
    }
}

int AirinFile::flushBFL1() {
    int written = 0;
    unsigned char offsetBuf[sizeof fsize];
    AirinCell *l2Cells[idxOnWriteBFL1->size()]; // L1's BFs Blocks -> L2' Cells

    for (int i = 0; i < idxOnWriteBFL1->size(); i++) {
        AirinBlockIndex *idxBlock = idxOnWriteBFL1->at(i);
        memcpy(offsetBuf, &fsize, sizeof fsize);
        written += writeBLK(idxBlock);
        l2Cells[i] = new AirinCell(idxBlock->cells->at(0)->key, offsetBuf, idxBlock->cells->at(0)->klen, sizeof fsize);
    }

    AirinBlockIndex *l2BFBlk;
    if (idxOnWriteBFL2->empty()) {
        l2BFBlk = new AirinBlockIndex(2);
        idxOnWriteBFL2->push_back(l2BFBlk);
    } else {
        l2BFBlk = idxOnWriteBFL2->back();
    }

    for (int i = 0; i < idxOnWriteBFL1->size(); i++) {
        if (l2BFBlk->size() >= INDEX_BLOCK_SIZE_BYTES) {
            idxSizeOnWriteL2 += l2BFBlk->size();
            l2BFBlk = new AirinBlockIndex(2);
            idxOnWriteBFL2->push_back(l2BFBlk);
        }
        l2BFBlk->put(l2Cells[i]);
    }

    for (int i = 0; i < idxOnWriteBFL1->size(); i++) {
        delete idxOnWriteBFL1->at(i);
    }
    idxSizeOnWriteBFL1 = 0;
    idxOnWriteBFL1->clear();

    curIdxBlk = new AirinBlockIndex(1);

    return written;
}

int AirinFile::flushBFL2() {
    int written = 0;
    for (int i = 0; i < idxOnWriteBFL2->size(); i++) {
        AirinBlockIndex *idxBlock = idxOnWriteBFL2->at(i);
        written += writeBLK(idxBlock);
    }

    for (int i = 0; i < idxOnWriteBFL2->size(); i++) {
        delete idxOnWriteBFL2->at(i);
    }
    idxSizeOnWriteBFL2 = 0;
    idxOnWriteBFL2->clear();
    return written;
}

int AirinFile::flushL1() {
    int written = 0;
    unsigned char offsetBuf[sizeof fsize];
    AirinCell *l2Cells[idxOnWriteL1->size()]; // L1's Blocks -> L2' Cells

    for (int i = 0; i < idxOnWriteL1->size(); i++) {
        AirinBlockIndex *idxBlock = idxOnWriteL1->at(i);
        memcpy(offsetBuf, &fsize, sizeof fsize);
        written += writeBLK(idxBlock);
        l2Cells[i] = new AirinCell(idxBlock->cells->at(0)->key, offsetBuf, idxBlock->cells->at(0)->klen, sizeof fsize);
    }

    AirinBlockIndex *l2Blk;
    if (idxOnWriteL2->empty()) {
        l2Blk = new AirinBlockIndex(2);
        idxOnWriteL2->push_back(l2Blk);
    } else {
        l2Blk = idxOnWriteL2->back();
    }

    for (int i = 0; i < idxOnWriteL1->size(); i++) {
        if (l2Blk->size() >= INDEX_BLOCK_SIZE_BYTES) {
            idxSizeOnWriteL2 += l2Blk->size();
            l2Blk = new AirinBlockIndex(2);
            idxOnWriteL2->push_back(l2Blk);
        }
        l2Blk->put(l2Cells[i]);
    }

    for (int i = 0; i < idxOnWriteL1->size(); i++) {
        delete idxOnWriteL1->at(i);
    }
    idxSizeOnWriteL1 = 0;
    idxOnWriteL1->clear();

    curIdxBlk = new AirinBlockIndex(1);

    return written;
}

int AirinFile::flushL2() {
    int written = 0;
    unsigned char offsetBuf[sizeof fsize];
    AirinCell *l3Cells[idxOnWriteL2->size()]; // L2's Blocks -> L3' Cells

    for (int i = 0; i < idxOnWriteL2->size(); i++) {
        AirinBlockIndex *idxBlock = idxOnWriteL2->at(i);
        memcpy(offsetBuf, &fsize, sizeof fsize);
        written += writeBLK(idxBlock);
        l3Cells[i] = new AirinCell(idxBlock->cells->at(0)->key, offsetBuf, idxBlock->cells->at(0)->klen, sizeof fsize);
    }

    AirinBlockIndex *l3Blk;
    if (idxOnWriteL3->empty()) {
        l3Blk = new AirinBlockIndex(3);
        idxOnWriteL3->push_back(l3Blk);
    } else {
        l3Blk = idxOnWriteL3->back();
    }

    for (int i = 0; i < idxOnWriteL2->size(); i++) {
        if (l3Blk->size() >= INDEX_BLOCK_SIZE_BYTES) {
            idxSizeOnWriteL3 += l3Blk->size();
            l3Blk = new AirinBlockIndex(3);
            idxOnWriteL3->push_back(l3Blk);
        }
        l3Blk->put(l3Cells[i]);
    }

    for (int i = 0; i < idxOnWriteL2->size(); i++) {
        delete idxOnWriteL2->at(i);
    }
    idxSizeOnWriteL2 = 0;
    idxOnWriteL2->clear();

    return written;
}

int AirinFile::flushL3() {
    int written = 0;
    for (int i = 0; i < idxOnWriteL3->size(); i++) {
        AirinBlockIndex *idxBlock = idxOnWriteL3->at(i);
        written += writeBLK(idxBlock);
    }

    for (int i = 0; i < idxOnWriteL3->size(); i++) {
        delete idxOnWriteL3->at(i);
    }
    idxSizeOnWriteL3 = 0;
    idxOnWriteL3->clear();
    return written;
}

void AirinFile::open(char *modes) {
    fp = fopen(filename, modes);
    fd = fileno(fp);
    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    raw = (unsigned char *) mmap(NULL, fsize, PROT_READ, MAP_SHARED, fd, 0);

    if (fsize == 0) {
        return;
    }

    long long offset = fsize;

    {
        offset-= sizeof bIdxBlkSize;
        unsigned char buf[sizeof bIdxBlkSize];
        pread(fd, buf, sizeof bIdxBlkSize, offset);
        memcpy(&bIdxBlkSize, buf, sizeof bIdxBlkSize);


        offset-= bIdxBlkSize;
        unsigned char *blkBuf = new unsigned char[bIdxBlkSize];
        pread(fd, blkBuf, bIdxBlkSize, offset);

        for (int i = 0; i < bIdxBlkSize; ) {
            AirinBlockAbstract *idxBlock = nullptr;

            int cSize = 0, dSize = 0, sizeOfSize = sizeof cSize + sizeof dSize;
            memcpy(&dSize, blkBuf + i, sizeof dSize);
            memcpy(&cSize, blkBuf + i + sizeof cSize, sizeof cSize);

            unsigned char decompressTmp[dSize];
            {
                size_t const _dSize = ZSTD_decompress(decompressTmp, dSize, blkBuf + i + sizeOfSize, cSize);
            }

            AirinBlockAbstract::read(decompressTmp, idxBlock);
            idxOnReadBF->push_back(idxBlock);
            i += sizeOfSize + cSize;
        }

        delete[] blkBuf;
    }

    {

        offset-= sizeof dIdxBlkSize;
        unsigned char buf[sizeof dIdxBlkSize];
        pread(fd, buf, sizeof dIdxBlkSize, offset);
        memcpy(&dIdxBlkSize, buf, sizeof dIdxBlkSize);

        offset-= dIdxBlkSize;
        unsigned char *blkBuf = new unsigned char[dIdxBlkSize];
        pread(fd, blkBuf, dIdxBlkSize, offset);

        for (int i = 0; i < dIdxBlkSize;) {
            AirinBlockIndex *idxBlock = nullptr;

            int cSize = 0, dSize = 0, sizeOfSize = sizeof cSize + sizeof dSize;
            memcpy(&dSize, blkBuf + i, sizeof dSize);
            memcpy(&cSize, blkBuf + i + sizeof cSize, sizeof cSize);

            unsigned char decompressTmp[dSize];
            {
                size_t const _dSize = ZSTD_decompress(decompressTmp, dSize, blkBuf + i + sizeOfSize, cSize);
            }

            AirinBlockAbstract::read(decompressTmp, reinterpret_cast<AirinBlockAbstract *&>(idxBlock));
            idxOnRead->push_back(idxBlock);
            i += sizeOfSize + cSize;
        }


        delete[] blkBuf;
    }
}

void AirinFile::close() {

    int dIdxWritten = 0, bIdxWritten = 0;

    if (!curIdxBlk->cells->empty()) {
        idxOnWriteL1->push_back(curIdxBlk);
    }

    // L1 的 size 可能不准，块级别的
    if (!idxOnWriteL1->empty()) {
        cout << "Level 1 Index Block Flushing (closing trigger)." << " ( " << endl;
        cout << "  ";
        idxOnWriteL1->front()->cells->front()->debug();
        cout << "  ";
        idxOnWriteL1->back()->cells->back()->debug();
        cout << ") size = " << idxSizeOnWriteL1 << endl;

        dIdxWritten = flushL1();
    }

    if (!idxOnWriteL2->empty() && idxOnWriteL2->at(0)->n > 1) {
        cout << "Level 2 Index Block Flushing (closing trigger)." << " ( " << endl;
        cout << "  ";
        idxOnWriteL2->front()->cells->front()->debug();
        cout << "  ";
        idxOnWriteL2->back()->cells->back()->debug();
        cout << ") size = " << idxSizeOnWriteL2 << endl;

        dIdxWritten = flushL2();
    }

    if (!idxOnWriteL3->empty() && idxOnWriteL3->at(0)->n > 1) {
        cout << "Level 3 Index Block Flushing (closing trigger)." << " ( " << endl;
        cout << "  ";
        idxOnWriteL3->front()->cells->front()->debug();
        cout << "  ";
        idxOnWriteL3->back()->cells->back()->debug();
        cout << ") size = " << idxSizeOnWriteL3 << endl;

        dIdxWritten = flushL3();
    }

    fwrite(&dIdxWritten, sizeof dIdxWritten, 1,fp);

    if (!curIdxBFBlk->cells->empty()) {
        idxOnWriteBFL1->push_back(curIdxBFBlk);
    }

    if (!idxOnWriteBFL1->empty()) {
        bIdxWritten = flushBFL1();
    }

    if (!idxOnWriteBFL2->empty() && idxOnWriteBFL2->at(0)->n > 1) {
        bIdxWritten = flushBFL2();
    }

    fwrite(&bIdxWritten, sizeof bIdxWritten, 1,fp);

    fclose(fp);

}

