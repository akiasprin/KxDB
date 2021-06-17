//
// Created by kevi on 2021/6/13.
//

#include "AirinStoreScanner.hpp"

AirinStoreScanner::AirinStoreScanner(AirinStore *_store) {
    store = _store;
    heap = new Heap<AirinFileScanner>();
    from = to = nullptr;
}

void AirinStoreScanner::init() {
    for (int i = 0; i < store->files->size(); i++) {
        AirinFileScanner *fScanner = new AirinFileScanner(store, store->files->at(i));
        fScanner->seek();
        heap->push(fScanner);
    }
}

void AirinStoreScanner::init(AirinCell *_from, AirinCell *_to, bool _useBloomFilter) {
    useBloomFilter = _useBloomFilter;
    from = _from;
    to = _to;

    for (int i = 0; i < store->files->size(); i++) {
        AirinFileScanner *fScanner = new AirinFileScanner(store, store->files->at(i));
        if (useBloomFilter) {
            if (!fScanner->contains(from)) {
                continue;;
            }
        }
        fScanner->seek(from);
        heap->push(fScanner);
    }
}

AirinCell* AirinStoreScanner::next() {
    if (heap->empty()) {
        return nullptr;
    }
    AirinFileScanner *fScanner = nullptr;
    heap->pop(fScanner);
    AirinCell *cell = fScanner->currentCell;
    if (to != NULL && to->_lessThan(cell)) {
        return nullptr;
    } else if (fScanner->next()) {
        heap->push(fScanner);
    }
    return cell;
}

void AirinStoreScanner::reset() {
    AirinFileScanner *e = nullptr;
    while (!heap->empty()) {
        heap->pop(e);
        delete e;
    }
}

void AirinStoreScanner::close() {
    reset();
    delete heap;
}
