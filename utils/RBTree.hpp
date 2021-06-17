//
//  RBTree.hpp
//  RBTree
//
//  Created by 葉朝鈞 on 2021/4/4.
//  Copyright © 2021 葉朝鈞. All rights reserved.
//

#ifndef RBTree_hpp
#define RBTree_hpp

#include <stdio.h>
#include "RBTreeNode.hpp"
#include "../AirinSE/AirinFilePopulation.hpp"

template <class T>
class RBTree {
private:
public:

    RBTreeNode<T> *root;
    RBTree() {
        this->root = NULL;
    }
    
    void insert(T *k) {
        if (root == NULL)
            root = new RBTreeNode<T>(k);
        else
            root->insert(root, k, false);
        root->color = BLACK;
    };
    
    void remove(T *k) {
        root->remove(root, k);
    }

    void removeMax(RBTreeNode<T> *&p, T *&k) {
        RBTreeNode<T> *max = root->maxNode();
        p = max->p;
        root->remove(root, max->k);
    }

    bool search(T *k, T *&ret) {
        if (root == NULL) {
            return false;
        }
        RBTreeNode<T> *it = root;
        
        while (true) {
            if (k->_equals((T *) it->k)) {
                ret = it->k;
                return true;
            } else if (k->_lessThan((T *) it->k)) {
                it = it->l;
            } else {
                it = it->r;
            }
            if (it == NULL) {
                return false;
            }
        }
    }

    void destroy(RBTreeNode<T> *rt) {
        if (rt == NULL) {
            return;
        }
        if (rt->l != NULL) {
            destroy(rt->l);
        }
        if (rt->r != NULL) {
            destroy(rt->r);
        }
        delete rt;
    }

    void visit(void (* callback)(T *, AirinFilePopulation *&pop, AirinFile *file), RBTreeNode<T> *rt, AirinFilePopulation *&pop, AirinFile *file) {
        if (rt->l != NULL) {
            visit(callback, rt->l, pop, file);
        }
        callback((T *) rt->k, pop, file);
        if (rt->r != NULL) {
            visit(callback, rt->r, pop, file);
        }
    }

    friend class RBTreeNode<T>;
};

#endif /* RBTree_hpp */
