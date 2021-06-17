//
//  AirinFile.hpp
//  KxDB
//
//  Created by 葉朝鈞 on 2021/6/4.
//

#ifndef AirinFile_hpp
#define AirinFile_hpp

#include <stdio.h>
#include <vector>

#include <iostream>
#include <cstring>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <zstd.h>
#include <algorithm>

#include "AirinBlockIndex.hpp"
#include "AirinBlockData.hpp"
#include "AirinBlockBloomFilter.hpp"
#include "../utils/Constant.hpp"

using namespace std;

class AirinFile {
private:
    int fd;
    FILE *fp;
    char *filename;
    unsigned char *raw; // For debug only

    int writeBLK(AirinBlockAbstract *block);

public:

    unsigned long long fsize;

    int dIdxBlkSize;
    int bIdxBlkSize;

    AirinBlockIndex *curIdxBlk;
    vector<AirinBlockIndex *> *idxOnWriteL1;
    vector<AirinBlockIndex *> *idxOnWriteL2;
    vector<AirinBlockIndex *> *idxOnWriteL3;
    int idxSizeOnWriteL1;
    int idxSizeOnWriteL2;
    int idxSizeOnWriteL3;
    
    AirinBlockIndex *curIdxBFBlk;
    vector<AirinBlockIndex *> *idxOnWriteBFL1;
    vector<AirinBlockIndex *> *idxOnWriteBFL2;
    int idxSizeOnWriteBFL1;
    int idxSizeOnWriteBFL2;

    vector<AirinBlockAbstract *> *idxOnRead;
    vector<AirinBlockAbstract *> *idxOnReadBF;

    AirinFile(char *filename);
    ~AirinFile();

    void getBLK(unsigned long long &offset, AirinBlockAbstract *&block, int &cSizeHint);
    void putBLK(AirinBlockData *block);
    
    void putBFBLK(AirinBlockBloomFilter *block);

    int flushL1();
    int flushL2();
    int flushL3();

    int flushBFL1();
    int flushBFL2();
    
    void open(char *modes);
    void close();
};

#endif /* AirinFile_hpp */
