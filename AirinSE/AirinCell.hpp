//
//  AirinCell.hpp
//  KxDB
//
//  Created by 葉朝鈞 on 2021/6/4.
//

#ifndef AirinCell_hpp
#define AirinCell_hpp

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <iostream>
#include <string.h>

#include "../utils/Comparable.hpp"

class AirinCell : public Comparable<AirinCell> {
private:
public:
    int klen, vlen;
    unsigned char *key, *val;
    time_t ts;

    AirinCell();
    AirinCell(AirinCell *);
    AirinCell(char *key, char *val, int klen, int vlen);
    AirinCell(unsigned char *key, unsigned char *val, int klen, int vlen);
    AirinCell(unsigned char *key, unsigned char *val, time_t ts, int klen, int vlen);
    ~AirinCell();

    int size();
    void write(unsigned char *buf);
    int read(unsigned char *buf);
    void debug();

    bool _lessThan(AirinCell *rhs) override;
    bool _equals(AirinCell *rhs) override;

    static bool lessThan(AirinCell *lhs, AirinCell *rhs);
    static bool equals(AirinCell *lhs, AirinCell *rhs);

    friend class AirinBlockData;
    friend class AirinBlockIndex;
    friend class AirinFile;

};
#endif /* AirinCell_hpp */
