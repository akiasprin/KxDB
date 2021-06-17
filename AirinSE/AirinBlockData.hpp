//
//  AirinBlockData.hpp
//  KxDB
//
//  Created by 葉朝鈞 on 2021/6/4.
//

#ifndef AirinBlock_hpp
#define AirinBlock_hpp

#include <stdio.h>
#include <vector>
#include "AirinCell.hpp"
#include "AirinBlockAbstract.hpp"
#include "../utils/Comparable.hpp"

using namespace std;

class AirinBlockData : virtual public AirinBlockAbstract, virtual public Comparable<AirinBlockData> {
private:
public:
    int8_t id;
    int n;
    vector<AirinCell *> *cells;

    AirinBlockData();
    AirinBlockData(int n, vector<AirinCell *> *);
    AirinBlockData(AirinBlockData *x);
    ~AirinBlockData() override;
    
    void put(AirinCell *) override;
    void find(AirinCell *) override;

    void writeBLK(unsigned char *buf) override;
    void writeBFBLK(unsigned char *buf) override {} ;

    int readBLK(unsigned char *buf) override;
    int readBFBLK(unsigned char *buf) override { return 0; } ;
    
    int size() override;
    void reset();

    void debug();

    virtual int8_t type() override { return id; };
    
    static bool lessThan(AirinBlockData *lhs, AirinBlockData *rhs);
    static bool equals(AirinBlockData *lhs, AirinBlockData *rhs);

    bool _lessThan(AirinBlockData *rhs) override;
    bool _equals(AirinBlockData *rhs) override;

};

#endif /* AirinBlock_hpp */
