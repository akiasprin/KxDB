//
//  RBTreeNode.hpp
//  RBTree
//
//  Created by 葉朝鈞 on 2021/4/4.
//  Copyright © 2021 葉朝鈞. All rights reserved.
//

#ifndef RBTreeNode_hpp
#define RBTreeNode_hpp

#include <stdio.h>
#include <algorithm>

using namespace std;

// 在2-3-4树模型下，黑色可理解为中间结点，红色为边缘结点
enum COLOR { RED, BLACK };

template <class T>
class RBTreeNode {
private:
    void fixAfterInsertion(RBTreeNode *&root, T *&_k, bool leaf) {
        if (p == NULL || p->p == NULL) return;
        if (color == BLACK || p->color == BLACK) return;

        RBTreeNode *g = p->p;
        bool LX = (g->l == p);
        RBTreeNode *u = LX ? g->r : g->l; // uncle

        if (u == NULL || u->color == BLACK) { // 空结点被视为黑色结点
            bool XL = (p->l == this);
            if (LX == XL) { // 同向，即 LL 或者 RR
                if (LX)
                    // LL: 父节点红色，祖父与叔叔结点黑色，父结点上升，并作为中间字，祖父结点作为边缘字下降（对应右旋），颜色互换
                    p->rightRotate();
                else
                    // RR: 同上，左旋
                    p->leftRotate();
                if (g == root)
                    root = p;
            } else {
                if (LX)
                    // LR: 转换为 LL 型（最终结果是，当前结点作为中间字，父和祖父节点作为边缘字，不等价转换也可）
                    leftRotate();
                else
                    // RL: 转换为 RR 型
                    rightRotate();
            }
        } else {
            // 标准的4结点（父结点与叔叔结点为红色，祖父节点黑色）分裂
            g->color = RED;
            p->color = u->color = BLACK;
        }
    }
    void fixAfterDeletion(RBTreeNode *&root, T *&_k) {
        RBTreeNode *c = this;

        for (;;) {
            RBTreeNode *p = c->p;
            if (p == NULL)
                break;
            bool L = (p->l == c);
            RBTreeNode *b = L ? p->r : p->l; // 兄弟结点，迭代的目的就是是把兄弟的黑色树高-1

            if (c->color == RED)
                break;

            if (L) {
                if (b->color == BLACK) {
                    if ((b->l == NULL || b->l->color == BLACK) && (b->r == NULL || b->r->color == BLACK)) {
                        // 可直接换色情况
                        if (p->color == BLACK) {
                            b->color = RED;
                            c = c->p;
                        } else {
                            swap(p->color, b->color);
                            break;
                        }
                    } else if (b->r != NULL && b->r->color == RED) {
                        // 兄弟结点的右结点是红色，兄弟结点左旋后，左子树黑色树高+1，右结点把红色变成黑色，即平衡
                        b->r->color = BLACK;
                        b->leftRotate();
                        break;
                    } else if (b->l != NULL && b->l->color == RED) {
                        // 旋转，转换上面的情况
                        b->l->rightRotate();
                    }
                } else {
                    // 规避兄弟结点红色情况
                    b->leftRotate();
                }
            } else {
                if (b->color == BLACK) {
                    if ((b->l == NULL || b->l->color == BLACK) && (b->r == NULL || b->r->color == BLACK)) {
                        if (p->color == BLACK) {
                            b->color = RED;
                            c = c->p;
                        } else {
                            swap(p->color, b->color);
                            break;
                        }
                    } else if (b->l != NULL && b->l->color == RED) {
                        b->l->color = BLACK;
                        b->rightRotate();
                        break;
                    } else if (b->r != NULL && b->r->color == RED) {
                        b->r->leftRotate();
                    }
                } else {
                    b->rightRotate();
                }
            }
        }

        if (p != NULL) {
            if (p->l == this)  p->l = NULL;
            else               p->r = NULL;
            while (root->p != NULL)
                root = root->p;
        } else  {
            root = NULL;
        }
        delete this;
    }
    
public:
    COLOR color;

    T *k;

    RBTreeNode *l, *r, *p;

    RBTreeNode() {
    }
    
    RBTreeNode(T *&_k) {
        this->k = _k;
        this->l = this->r = this->p = NULL;
        this->color = RED;
    }

    RBTreeNode(T *&_k, RBTreeNode *p) {
        this->k = _k;
        this->l = this->r = NULL;
        this->p = p;
        this->color = RED;
    }

    RBTreeNode* maxNode() {
        RBTreeNode *it = this;
        while (it->r != NULL)
            it = it->r;
        return it;
    }

    RBTreeNode* minNode() {
        RBTreeNode *it = this;
        while (it->l != NULL)
            it = it->l;
        return it;
    }
    
    void insert(RBTreeNode *&root, T *&_k, bool leaf) {
        if (!leaf) {
            if (_k->_lessThan(k)) {
                if ((leaf = (l == NULL)))
                    l = new RBTreeNode(_k, this);
                l->insert(root, _k, leaf);
            } else {
                if ((leaf = (r == NULL)))
                    r = new RBTreeNode(_k, this);
                r->insert(root, _k, leaf);
            }
        }
        fixAfterInsertion(root, _k, leaf);
    }

    void remove(RBTreeNode *&root, T *&_k) {
        if (_k->_equals(k)) {
            if (l != NULL) {
                RBTreeNode *tmp = l->maxNode();
                k = tmp->k;
                l->remove(root, tmp->k);
            } else if (r != NULL) {
                RBTreeNode *tmp = r->minNode();
                k = tmp->k;
                r->remove(root, tmp->k);
            } else {
                fixAfterDeletion(root, k);
            }
        } else if (_k->_lessThan(k)) {
            if (l == NULL)
                throw exception();
            l->remove(root, _k);
        } else {
            if (r == NULL)
                throw exception();
            r->remove(root, _k);
        }
    }
    
    void leftRotate() {
        RBTreeNode *c = this;
        RBTreeNode *g = p->p;
        // 先绑定祖父结点的孩子为当前结点，优先是因为无利用情况在后面
        if (g != NULL)
            g->l == p ? g->l = c : g->r = c;
        // 然后改父节点的右孩子，之前记录是当前结点，显然无效记录，优先修改；按照顺序把当前的左孩子挂载上去
        p->r = c->l;
        // 更新左孩子的父结点信息s
        if (p->r != NULL)
            p->r->p = p;
        // 原父结点下拉后，更新其父结点信息，同样是无效信息优先修改
        p->p = c;
        // 把原父结点挂载到左子树
        c->l = p; // 原 l 指针含义失效
        c->p = g; // 原 p 指针含义失效
        // 颜色转换；原父结点如果是中间结点，经过旋转父结点转为左边缘结点；同理，当前结点最终作为了中间结点
        swap(c->color, c->l->color);
    }
    void rightRotate() {
        RBTreeNode *c = this;
        RBTreeNode *g = p->p;
        if (g != NULL)
            g->l == p ? g->l = c : g->r = c;
        p->l = c->r;
        if (p->l != NULL)
            p->l->p = p;
        p->p = c;
        c->r = p;
        c->p = g;
        swap(c->color, c->r->color);
    }
};
#endif /* RBTreeNode_hpp */
