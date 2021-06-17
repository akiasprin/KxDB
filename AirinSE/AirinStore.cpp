//
// Created by kevi on 2021/6/11.
//

#include "AirinStore.hpp"

AirinStore::AirinStore() {
    files = new vector<AirinFile *>();
    cache = new LRU<CacheKey, CacheValue *>(BLOCK_CACHE_SIZE_BYTES);
}

void AirinStore::init(char *p) {
    DIR *dir = opendir(p);
    if (dir == NULL) return;
    class dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        char filename[1024];
        if (strstr(entry->d_name, ".kx") != NULL) {
            sprintf(filename, "%s/%s", DB_DATA_PATH, entry->d_name);
            AirinFile *f = new AirinFile(filename);
            f->open("r");
            files->push_back(f);
        }
    }
    closedir(dir);
}

void AirinStore::getBLK(AirinFile *file, unsigned long long &offset, AirinBlockAbstract *&block, int &cSizeHint) {
    CacheKey key = CacheKey(file, offset);
    CacheValue *val = cache->get(key);
    if (val == nullptr) {
        val = new CacheValue();
        file->getBLK(offset, (AirinBlockAbstract *&) val->blk, val->cSizeHint);
        cache->add(key, val, val->blk->size());
    }
    block = val->blk;
    cSizeHint = val->cSizeHint;
}

bool AirinStore::getRearDataBLK(AirinFile *file, unsigned long long &offset, AirinBlockData *&block, int &cSizeHint) {
    while (true) {
        offset += sizeof(int) + sizeof(int) + cSizeHint;
        if (offset < file->fsize - file->dIdxBlkSize - file->bIdxBlkSize - sizeof(int) - sizeof(int)) {
            block = nullptr;
            getBLK(file, offset, (AirinBlockAbstract *&) block, cSizeHint);
            if (block->id == 0) {
                return true;
            } else {
                release(file, offset);
            }
        } else {
            return false;
        }
    }
}

void AirinStore::release(AirinFile *file, unsigned long long offset) {
    CacheKey key = CacheKey(file, offset);
    cache->release(key);
}
