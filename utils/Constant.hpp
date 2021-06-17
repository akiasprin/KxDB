//
//  constant.hpp
//  KxDB
//
//  Created by 葉朝鈞 on 2021/6/8.
//

#ifndef constant_hpp
#define constant_hpp

#include <stdio.h>

const int DATA_BLOCK_SIZE_BYTES                  = 128 * 1024;
const int INDEX_BLOCK_SIZE_BYTES                 = 128 * 1024;
const int FIRST_IDX_LEVEL_INMEMORY_SIZE_BYTES    = 128 * 1024;
const int SECOND_IDX_LEVEL_INMEMORY_SIZE_BYTES   = 128 * 1024;
const long long BLOCK_CACHE_SIZE_BYTES           = 1 * 1024 * 1024;
const long long MEMSTORE_SIZE_BYTES              = 256 * 1024 * 1024;

const int BIT_MAP_LEN = 32 * 1024 << 6;
const int LL_ARRAY_LEN = BIT_MAP_LEN >> 6;       // LL 8 bytes * (32 * 1024) => 256K
const int LL_MOD_MASK = (1 << 6) - 1;
const int BIT_MAP_SIZE_BYTES = BIT_MAP_LEN >> 3; // 8 bits = 1 byte
const int BLOOM_FILTER_ELEMENT_NUM = 128 * 1024; // BIT_MAP_LEN / 1.44 * 5  * 4?

const int BLOCK_TYPE_BLOOM_FILTER        = -1;
const int BLOCK_TYPE_DATA                = 0;
const int BLOCK_TYPE_INDEX_FIRST_LEVEL   = 1;
const int BLOCK_TYPE_INDEX_SECOND_LEVEL  = 2;
const int BLOCK_TYPE_INDEX_THIRD_LEVEL   = 3;

const char DB_DATA_PATH[] = "/tmp/airin/db";

#endif /* constant_hpp */
