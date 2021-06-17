//
//  AirinBlockData.cpp
//  KxDB
//
//  Created by 葉朝鈞 on 2021/6/4.
//

#include <iostream>
#include <string.h>
#include "AirinBlockData.hpp"

using namespace std;

AirinBlockData::AirinBlockData() {
    n = 0;
    id = 0;
    cells = new vector<AirinCell *>;
}

AirinBlockData::AirinBlockData(int _n, vector<AirinCell *> *_cells) {
    n = _n;
    id = 0;
    cells = _cells;
}

AirinBlockData::AirinBlockData(AirinBlockData *x) {
    n = x->n;
    id = x->id;
    cells = new vector<AirinCell *>;
    cells->resize(n);
    for (int i = 0; i < n; i++) {
        cells->at(i) = new AirinCell(x->cells->at(i));
    }
}

AirinBlockData::~AirinBlockData() {
    for (int i = 0; i < n; i++)
        delete cells->at(i);
    cells->clear();
    delete cells;
}

int AirinBlockData::size() {
    int ret = sizeof id + sizeof n;
    for (int i = 0; i < n; i++) {
        ret += cells->at(i)->size();
    }
    return ret;
}

void AirinBlockData::reset() {
    for (int i = 0; i < n; i++)
        delete cells->at(i);
    n = 0;
    id = 0;
    cells->clear();
}

void AirinBlockData::put(AirinCell *cell) {
    n++;
    cells->push_back(cell);
}

void AirinBlockData::find(AirinCell *cell) {
    
}

void AirinBlockData::writeBLK(unsigned char *buf) {
    memcpy(buf, &id, sizeof id);
    buf += sizeof id;
    memcpy(buf, &n, sizeof n);
    buf += sizeof n;
    for (int i = 0, j = 0; i < n; i++) {
        int blockSize = cells->at(i)->size();
        cells->at(i)->write(buf + j);
        j += blockSize;
    }
}

int AirinBlockData::readBLK(unsigned char *buf) {
    memcpy(&id, buf, sizeof id);
    buf += sizeof id;
    memcpy(&n, buf, sizeof n);
    buf += sizeof n;
    cells->resize(n);
    for (int i = 0, j = 0; i < n; i++) {
        cells->at(i) = new AirinCell();
        j += cells->at(i)->read(buf + j);
    }
    return size();
}

void AirinBlockData::debug() {
    for (int i = 0; i < n; i++) {
        AirinCell *cell = cells->at(i);
        cout << "K:";
        for (int j = 0; j < cell->klen; j++) {
            cout << cell->key[j];
        }
        cout << ", V:" << cell->vlen << " bytes";
        cout << endl;
    }
}

bool AirinBlockData::_lessThan(AirinBlockData *rhs) {
    return cells->at(0)->_lessThan(rhs->cells->at(0));
}

bool AirinBlockData::_equals(AirinBlockData *rhs) {
    return cells->at(0)->_equals(rhs->cells->at(0));
}

bool AirinBlockData::lessThan(AirinBlockData *lhs, AirinBlockData *rhs) {
    return AirinCell::lessThan(lhs->cells->at(0), rhs->cells->at(0));
}

bool AirinBlockData::equals(AirinBlockData *lhs, AirinBlockData *rhs) {
    return AirinCell::equals(lhs->cells->at(0), rhs->cells->at(0));
}

