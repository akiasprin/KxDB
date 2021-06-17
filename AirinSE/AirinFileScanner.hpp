//
// Created by kevi on 2021/6/13.
//

#ifndef KXDB_AIRINFILESCANNER_HPP
#define KXDB_AIRINFILESCANNER_HPP


#include "AirinFile.hpp"
#include "AirinStore.hpp"
#include "../utils/Helper.hpp"

class AirinFileScanner : public Comparable<AirinFileScanner> {
private:
    int i, cSizeHint;
    unsigned long long offset, oldOffset;
    AirinFile *f;
    AirinStore *store;
    AirinBlockData *blk;

public:
    AirinCell *currentCell;

    AirinFileScanner(AirinStore *_store, AirinFile *_f);

    void seek();
    void seek(AirinCell *keywordCell);
    AirinCell* next();

    bool contains(AirinCell *keywordCell);

    static bool lessThan(AirinFileScanner *lhs, AirinFileScanner *rhs);
    static bool equals(AirinFileScanner *lhs, AirinFileScanner *rhs);

    bool _lessThan(AirinFileScanner *rhs) override;
    bool _equals(AirinFileScanner *rhs) override;
};


#endif //KXDB_AIRINFILESCANNER_HPP
