//
//  main.cpp
//  KxDB
//
//  Created by 葉朝鈞 on 2021/6/4.
//

#include <iostream>
#include <algorithm>
#include <vector>

#include "AirinSE/AirinStore.hpp"
#include "AirinSE/AirinStoreScanner.hpp"
#include "utils/Constant.hpp"
#include "AirinSE/AirinMem.hpp"

#include "test/io.cpp"
#include "test/bloom.cpp"

#define MAXN 3000000

using namespace std;



int main(int argc, const char * argv[]) {
    srand(1);

    IO_TEST *t1 = new IO_TEST();

    AirinMem *mem = new AirinMem();
    t1->fillMemStore(mem, MAXN);
    mem->close();

    AirinStore *store = new AirinStore();
    store->init((char *) DB_DATA_PATH);

    t1->compact(store);

    t1->scan(store, "0", "1", 10);
    t1->scan(store, "\1\0", "\255\0", -1, 1);
    t1->scan(store, "\1\0", "\255\0", -1, 2);

    t1->get(store, "ffd94ab963b461cd8742f44656afec5c");
    t1->get(store, "7b3cebd691d4965a31ab5fd78b55cbbe");
    t1->get(store, "2ee264ff03d12477bc501c558f1c5bf6");

    BLOOM_FILTER_TEST* t2 = new BLOOM_FILTER_TEST();

    t2->sampling(10);

    return 0;
}
