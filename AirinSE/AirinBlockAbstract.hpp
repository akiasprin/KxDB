//
//  AirinBlockAbstract.hpp
//  KxDB
//
//  Created by 葉朝鈞 on 2021/6/16.
//

#ifndef AirinBlockInterface_hpp
#define AirinBlockInterface_hpp

#include <iostream>

#include "AirinCell.hpp"

class AirinBlockAbstract {
private:
    static int8_t getTypeId(unsigned char *buf);
public:
    
    virtual ~AirinBlockAbstract() {};
    
    virtual void put(AirinCell *) {};
    virtual void find(AirinCell *) {};

    static int read(unsigned char *buf, AirinBlockAbstract *&blk);
    void write(unsigned char *buf);

    virtual void writeBLK(unsigned char *buf) {};
    virtual void writeBFBLK(unsigned char *buf) {};

    virtual int readBLK(unsigned char *buf) { return 0; };
    virtual int readBFBLK(unsigned char *buf) { return 0; };
    
    virtual int size() { return 0; };
    virtual int8_t type() { return 0; };
};

#endif /* AirinBlockInterface_hpp */
