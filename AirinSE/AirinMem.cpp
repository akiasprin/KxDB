//
// Created by kevi on 2021/6/11.
//

#include "AirinMem.hpp"

AirinMem::AirinMem() {
    size = 0ULL;
    tree = NULL;
}

void AirinMem::add(AirinCell *cell) {
    if (tree == NULL) {
        tree = new RBTree<AirinCell>();
    }
    tree->insert(cell);
    size += cell->size();

    if (size >= MEMSTORE_SIZE_BYTES) {
        cout << "Memory store in-memory has reached full capacity, to async flush.." << endl;
        size = 0ULL;
        while (flushFuture.valid() && !Helper::is_ready(flushFuture)) {
            cout << "Waiting for last memory store flush be completed.." << endl;
            flushFuture.wait();
        }
        RBTree<AirinCell> *_tree = tree;
        flushFuture = std::async(std::launch::async, [_tree]() {
            flush(_tree);
        });
        tree = NULL;
    }
}

void AirinMem::flush(RBTree<AirinCell> *tree) {
    char filename[1024];
    sprintf(filename, "%s/%ld_%lld.%s", DB_DATA_PATH, time(0), Helper::getRandom(0, (1LL << 60)), "kx");
    AirinFile *file = new AirinFile(filename);
    file->open("w+");
    AirinFilePopulation *pop = new AirinFilePopulation(file);
    tree->visit(flushCallback, tree->root, pop, file);
    pop->close();
    file->close();
    tree->destroy(tree->root);
    delete file;
    delete tree;
}

void AirinMem::flushCallback(AirinCell *cell, AirinFilePopulation *&pop, AirinFile *file) {
    pop->add(cell);
}

void AirinMem::close() {
    flush(tree);
    while (flushFuture.valid() && !Helper::is_ready(flushFuture)) {
        cout << "Waiting for last memory store flush be completed.." << endl;
        flushFuture.wait();
    }
    cout << "Memory store has been shut down" << endl;
}

AirinMem::~AirinMem() {
}
