//
//  AirinBlockIndex.cpp
//  KxDB
//
//  Created by 葉朝鈞 on 2021/6/5.
//

#include <cstring>
#include "AirinBlockIndex.hpp"

AirinBlockIndex::AirinBlockIndex(int level) : AirinBlockData() {
    id = level;
}

AirinBlockIndex::AirinBlockIndex(int level, int n, vector<struct AirinCell *> *cells) : AirinBlockData(n, cells) {
    id = level;
}

void AirinBlockIndex::getOffsetFromIndexCell(AirinCell *cell, unsigned long long &offset) {
    memcpy(&offset, cell->val, cell->vlen);
}
