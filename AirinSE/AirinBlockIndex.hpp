//
//  AirinBlockIndex.hpp
//  KxDB
//
//  Created by 葉朝鈞 on 2021/6/5.
//

#ifndef AirinIndex_hpp
#define AirinIndex_hpp

#include <stdio.h>
#include "AirinBlockData.hpp"

class AirinBlockIndex : public AirinBlockData {
public:
    AirinBlockIndex(int level);
    AirinBlockIndex(int level, int n, vector<struct AirinCell *> *cells);

    static void getOffsetFromIndexCell(AirinCell *cell, unsigned long long &offset);
};

#endif /* AirinIndex_hpp */
