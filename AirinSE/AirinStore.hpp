//
// Created by kevi on 2021/6/11.
//

#ifndef KXDB_AIRINSTORE_HPP
#define KXDB_AIRINSTORE_HPP

#include <string>
#include <dirent.h>
#include <sys/types.h>

#include "AirinFile.hpp"
#include "../utils/LRU.hpp"

class AirinStore {
public:
    class CacheKey {
    public:
        AirinFile *file;
        unsigned long long offset;

        CacheKey() {}
        CacheKey(AirinFile *_file, unsigned long long _offset) {
            file = _file;
            offset = _offset;
        }

        bool operator<(CacheKey &rhs) const {
            return file < rhs.file || (file == rhs.file && offset < rhs.offset);
        }
        bool operator==(CacheKey &rhs) const {
            return file == rhs.file && offset == rhs.offset;
        }
    };

    class CacheValue {
    public:
        int cSizeHint;
        AirinBlockAbstract *blk;

        CacheValue() {
            cSizeHint = 0;
            blk = nullptr;
        }

        ~CacheValue() {
            delete blk;
        }
    };

    vector<AirinFile *> *files;
    LRU<CacheKey, CacheValue *> *cache;

    AirinStore();

    void init(char *p);

    void getBLK(AirinFile* file, unsigned long long &offset, AirinBlockAbstract *&block, int &cSizeHint);
    bool getRearDataBLK(AirinFile* file, unsigned long long &offset, AirinBlockData *&block, int &cSizeHint);
    void release(AirinFile* file, unsigned long long offset);
    
};


#endif //KXDB_AIRINSTORE_HPP
