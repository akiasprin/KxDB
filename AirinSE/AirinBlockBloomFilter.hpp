//
//  AirinBlockBloomFilter.hpp
//  KxDB
//
//  Created by 葉朝鈞 on 2021/6/15.
//

#ifndef AirinBlockBloomFilter_hpp
#define AirinBlockBloomFilter_hpp

#include <stdio.h>

#include "AirinBlockData.hpp"
#include "../utils/Helper.hpp"
#include "../utils/Constant.hpp"
#include "../utils/BloomFilterFunctions.hpp"

class AirinBlockBloomFilter : virtual public AirinBlockAbstract {
private:
    void setBit(int p);
    bool getBit(int p);
    
    void insert(unsigned char *key, int klen);
    bool contains(unsigned char *key, int klen);
public:
    int8_t id;
    AirinCell *head;
    int m, n, k;
    unsigned long long *bitMap;

    AirinBlockBloomFilter();
    ~AirinBlockBloomFilter() override;
    
    int size() override;
    
    void put(AirinCell *cell) override;
    void find(AirinCell *cell) override;
    bool contains(AirinCell *cell);
    
    void reset();
    
    void writeBFBLK(unsigned char *buf) override;
    int readBFBLK(unsigned char *buf) override;

    virtual int8_t type() override { return id; };
};

#endif /* AirinBlockBloomFilter_hpp */
