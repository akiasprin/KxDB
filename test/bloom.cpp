//
// Created by kevi on 2021/6/15.
//

#include <iostream>
#include <algorithm>
#include <openssl/md5.h>
#include <unordered_set>
#include <cstring>

#include "../utils/Helper.hpp"
#include "../utils/Constant.hpp"
#include "../utils/BloomFilterFunctions.hpp"


using namespace std;

// Bloom Filter Predict: 1493, Real: 0 Accuracy: 99.8576%
// const int MAXN = 1024 * 1024;

// Bloom Filter Predict: 1175, Real: 0 Accuracy: 99.8825%

class BLOOM_FILTER_TEST {
private:
    unsigned long long bitMap[LL_ARRAY_LEN];
public:
    static void sprintMd5(unsigned char *md, char *ret) {
        for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
            sprintf(ret + (i << 1), "%02x", md[i]);
        }
    }

    void setBit(int p) {
        int b = p >> 6;
        int s = p & LL_MOD_MASK;
        bitMap[b] |= (1LL << s);
    }

    bool getBit(int p) {
        int b = p >> 6;
        int s = p & LL_MOD_MASK;
        return bitMap[b] & (1LL << s);
    }

    BLOOM_FILTER_TEST* sampling(int num) {

        for (int i = 1; i <= num; i++) {

            srand(time(0));
            memset(bitMap, 0, sizeof bitMap);

            cout << "Starting Bloom Filter, Round " << i << endl;

            unsigned char md[MD5_DIGEST_LENGTH];
            char strMd5[(MD5_DIGEST_LENGTH << 1) + 1];

            unordered_set<string> mySet;

            for (int j = 0; j < BLOOM_FILTER_ELEMENT_NUM; j++) {
                // prepare key
                ULL k = Helper::getRandom(0, (1LL << 60));
                MD5((const unsigned char *) &k, sizeof k, md);
                sprintMd5(md, strMd5);

                setBit(BloomFilterFunctions::BKDRHash((unsigned char *) strMd5, 32) % BIT_MAP_LEN);
                setBit(BloomFilterFunctions::JSHash((unsigned char *) strMd5, 32) % BIT_MAP_LEN);
                setBit(BloomFilterFunctions::DEKHash((unsigned char *) strMd5, 32) % BIT_MAP_LEN);
                setBit(BloomFilterFunctions::FNVHash((unsigned char *) strMd5, 32) % BIT_MAP_LEN);
                setBit(BloomFilterFunctions::APHash((unsigned char *) strMd5, 32) % BIT_MAP_LEN);

                mySet.insert(string(strMd5));
            }

            cout << "Generation Phase End" << endl;

            int bloom = 0, real = 0;
            for (int j = 0; j < BLOOM_FILTER_ELEMENT_NUM; j++) {
                // prepare key
                ULL k = Helper::getRandom(0, (1LL << 60));
                MD5((const unsigned char *) &k, sizeof k, md);
                sprintMd5(md, strMd5);

                if (getBit(BloomFilterFunctions::BKDRHash((unsigned char *) strMd5, 32) % BIT_MAP_LEN)
                        && getBit(BloomFilterFunctions::JSHash((unsigned char *) strMd5, 32) % BIT_MAP_LEN)
                        && getBit(BloomFilterFunctions::DEKHash((unsigned char *) strMd5, 32) % BIT_MAP_LEN)
                        && getBit(BloomFilterFunctions::FNVHash((unsigned char *) strMd5, 32) % BIT_MAP_LEN)
                        && getBit(BloomFilterFunctions::APHash((unsigned char *) strMd5, 32) % BIT_MAP_LEN)) {
                    bloom++;
                }

                if (mySet.find(string(strMd5)) != mySet.end()) {
                    real++;
                }
            }

            cout << "Bloom Filter Predict: " << bloom << ", Real: " << real << " Accuracy: " << 100.0 * (BLOOM_FILTER_ELEMENT_NUM - bloom + real) / BLOOM_FILTER_ELEMENT_NUM << "%" << endl;
        }

        return this;
    }
};
