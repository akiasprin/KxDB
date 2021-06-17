//
//  AirinBlockBloomFilter.cpp
//  KxDB
//
//  Created by 葉朝鈞 on 2021/6/15.
//

#include "AirinBlockBloomFilter.hpp"

AirinBlockBloomFilter::AirinBlockBloomFilter() {
    id = -1;
    head = NULL;
    m = BIT_MAP_LEN; n = 0; k = 5;
    bitMap = new unsigned long long[BIT_MAP_LEN >> 6];
}

AirinBlockBloomFilter::~AirinBlockBloomFilter() {
    delete bitMap;
}

int AirinBlockBloomFilter::size()  {
    return sizeof id + sizeof m + sizeof n + sizeof k + (m >> 3);
}

void AirinBlockBloomFilter::setBit(int p)  {
    int b = p >> 6;
    int s = p & LL_MOD_MASK;
    bitMap[b] |= (1LL << s);
}

bool AirinBlockBloomFilter::getBit(int p)  {
    int b = p >> 6;
    int s = p & LL_MOD_MASK;
    return bitMap[b] & (1LL << s);
}

void AirinBlockBloomFilter::insert(unsigned char *key, int klen) {
    setBit(BloomFilterFunctions::BKDRHash((unsigned char *) key, klen) % BIT_MAP_LEN);
    setBit(BloomFilterFunctions::DEKHash((unsigned char *) key, klen) % BIT_MAP_LEN);
    setBit(BloomFilterFunctions::FNVHash((unsigned char *) key, klen) % BIT_MAP_LEN);
    setBit(BloomFilterFunctions::JSHash((unsigned char *) key, klen) % BIT_MAP_LEN);
    setBit(BloomFilterFunctions::APHash((unsigned char *) key, klen) % BIT_MAP_LEN);
}

bool AirinBlockBloomFilter::contains(unsigned char *key, int klen) {
    if (getBit(BloomFilterFunctions::BKDRHash((unsigned char *) key, klen) % BIT_MAP_LEN)
        && getBit(BloomFilterFunctions::DEKHash((unsigned char *) key, klen) % BIT_MAP_LEN)
        && getBit(BloomFilterFunctions::FNVHash((unsigned char *) key, klen) % BIT_MAP_LEN)
        && getBit(BloomFilterFunctions::JSHash((unsigned char *) key, klen) % BIT_MAP_LEN)
        && getBit(BloomFilterFunctions::APHash((unsigned char *) key, klen) % BIT_MAP_LEN)) {
        return true;
    }
    return false;
}

void AirinBlockBloomFilter::put(AirinCell *cell) {
    if (n++ == 0) {
        head = new AirinCell(cell->key, NULL, cell->klen, 0);
    }
    insert(cell->key, cell->klen);
}

void AirinBlockBloomFilter::find(AirinCell *cell) {
    contains(cell->key, cell->klen);
}

bool AirinBlockBloomFilter::contains(AirinCell *cell) {
    return contains(cell->key, cell->klen);
}

void AirinBlockBloomFilter::writeBFBLK(unsigned char *buf) {
    memcpy(buf, &id, sizeof id);
    buf += sizeof id;
    memcpy(buf, &m, sizeof m);
    buf += sizeof m;
    memcpy(buf, &n, sizeof n);
    buf += sizeof n;
    memcpy(buf, &k, sizeof k);
    buf += sizeof k;
    memcpy(buf, bitMap, (m >> 3));
    buf += (m >> 3);
   // head->write(buf);
}

int AirinBlockBloomFilter::readBFBLK(unsigned char *buf) {
    int readSize = 0;
    memcpy(&id, buf + readSize, sizeof id);
    readSize += sizeof id;
    memcpy(&m, buf + readSize, sizeof m);
    readSize += sizeof m;
    memcpy(&n, buf + readSize, sizeof n);
    readSize += sizeof n;
    memcpy(&k, buf + readSize, sizeof k);
    readSize += sizeof k;
    bitMap = new unsigned long long[m >> 6];
    memcpy(bitMap, buf + readSize, (m >> 3));
    readSize += (m >> 3);
   // readSize += head->read(buf + readSize);
    return readSize;
}

void AirinBlockBloomFilter::reset() {
    delete head;
    head = NULL;
    id = -1;
    m = BIT_MAP_LEN; n = 0; k = 5;
    memset(bitMap, 0, (m >> 3));
}
