//
//  BloomFilterFunctions.hpp
//  KxDB
//
//  Created by 葉朝鈞 on 2021/6/15.
//

#ifndef BloomFilterFunctions_hpp
#define BloomFilterFunctions_hpp

#include <stdio.h>

class BloomFilterFunctions {
public:

    static unsigned int BKDRHash(unsigned char *ptr, unsigned char len) {
        unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
        unsigned int hash = 0;
        
        for (int i = 0; i < len; ptr++, i++) {
            hash = hash * seed + *ptr;
        }
        return (hash & 0x7FFFFFFF);
    }

    static unsigned int DEKHash(unsigned char* str, unsigned int len) {
        unsigned int hash = len;
        unsigned int i = 0;
        
        for (i = 0; i < len; str++, i++) {
            hash = ((hash << 5) ^ (hash >> 27)) ^ (*str);
        }
        return (hash & 0x7FFFFFFF);
    }

    static unsigned int FNVHash(unsigned char* str, unsigned int len) {
        const unsigned int fnv_prime = 0x811C9DC5;
        unsigned int hash = 0;
        unsigned int i = 0;
        
        for (i = 0; i < len; str++, i++) {
            hash *= fnv_prime;
            hash ^= (*str);
        }
        return (hash & 0x7FFFFFFF);
    }


    static unsigned int APHash(unsigned char* str, unsigned int len) {
        unsigned int hash = 0xAAAAAAAA;
        unsigned int i = 0;

        for (i = 0; i < len; str++, i++) {
            hash ^= ((i & 1) == 0) ? ((hash << 7) ^ (*str) * (hash >> 3)) :
                    (~((hash << 11) + (*str) ^ (hash >> 5)));
        }

        return (hash & 0x7FFFFFFF);
    }

    static unsigned int JSHash(unsigned char *ptr, unsigned char len) {
        unsigned int hash = 1315423911;
        unsigned int i = 0;

        for (i = 0; i < len; ptr++, i++) {
            hash ^= ((hash << 5) + (*ptr) + (hash >> 2));
        }
        return (hash & 0x7FFFFFFF);
    }


};

#endif /* BloomFilterFunctions_hpp */
