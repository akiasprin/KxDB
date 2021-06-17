//
//  Comparable.hpp
//  KxDB
//
//  Created by 葉朝鈞 on 2021/6/11.
//

#ifndef Comparable_hpp
#define Comparable_hpp

template <class T>
class Comparable {
public:
    virtual bool _lessThan(T *rhs) = 0;
    virtual bool _equals(T *rhs) = 0;
};

#endif /* Comparable_hpp */
