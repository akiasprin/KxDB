//
//  LRU.hpp
//  KxDB
//
//  Created by 葉朝鈞 on 2021/6/4.
//

#ifndef LRU_hpp
#define LRU_hpp

#include <stdio.h>

#include "RBTree.hpp"

using namespace std;

template<class K, class V>
class LRU {
private:
    class Node : public Comparable<Node> {
    public:
        K k;
        V v;
        int r;  // reference counters
        bool d; // evicted
        size_t size;
        Node *prev;
        Node *next;
        
        Node() {
            r = 0;
            d = false;
            size = 0;
            prev = next = NULL;
        }
        Node(K &_k) {
            k = _k;
            v = NULL;
            r = 0;
            d = false;
            size = 0;
            prev = next = NULL;
        }
        ~Node() {
            if (v != NULL) {
                delete v;
            }
        }
        
        bool _lessThan(Node *rhs) override {
            return k < rhs->k;
        }
        bool _equals(Node *rhs) override {
            return k == rhs->k;
        }
        
    };
    
    Node *head;
    Node *tail;
    
    size_t size;
    size_t maxSize;
    
    RBTree<Node> *m;
    RBTree<Node> *gc; // evict 的记录，可能会拉回链表里，需要删除集合

    void evict() {
        while (size > maxSize) {
            Node *prevTail = tail->prev;
            size -= tail->size;
            prevTail->next = nullptr;
            if (tail->r == 0) {
                m->remove(tail);
                delete tail;
            } else {
                tail->d = true;
                gc->insert(tail);
            }
            tail = prevTail;
        }
    }
    
public:
    LRU(size_t _size) {
        maxSize = _size;
        size = 0;
        head = tail = nullptr;
        m = new RBTree<Node>;
        gc = new RBTree<Node>;
    }

    void release(K &k) {
        Node key = Node(k);
        Node *stored = NULL;
        if (m->search(&key, (Node *&)stored)) {
            stored->r--;
            if (stored->r == 0 && stored->d) {
                if ((Node *)stored == tail) {
                    tail = stored->prev;
                }
                m->remove(stored);
                if (gc->search(stored, stored)) {
                    gc->remove(stored);
                }
                delete stored;
            }
        } else {
            throw exception();
        }
    }
    
    void add(K &k, V &v, size_t _size) {
        Node key = Node(k);
        Node *stored = NULL;
        if (!m->search(&key, (Node *&) stored)) {
            Node *node = new Node();
            node->k = k;
            node->v = v;
            node->size = _size;
            if (head == nullptr && tail == nullptr) {
                node->prev = node->next = nullptr;
                head = tail = node;
            } else {
                node->prev = NULL;
                node->next = head;
                head->prev = node;
                head = node;
            }
            m->insert(node);
            size += _size;
        } else {
            if (head == stored) {
                return;
            }
            // 拉回
            if (stored->d) {
                stored->prev = NULL;
                stored->next = head;
                head->prev = stored;
                head = stored;
                size += stored->size;
                stored->d = false;
                gc->remove(stored);
                return;
            }
            
            Node *prevNode = stored->prev;
            Node *nextNode = stored->next;
            prevNode->next = nextNode;
            if (nextNode != nullptr)
                nextNode->prev = prevNode;
            stored->next = head;
            stored->prev = nullptr;
            head->prev = stored;
            head = stored;
            if (tail == stored) {
                tail = prevNode;
            }
            stored->v = v;
            // 可能更新
            size = size - stored->size + _size;
            stored->size = _size;
        }
        head->r++;
        evict();
    }

    V get(K &k) {
        Node key = Node(k);
        Node *stored = NULL;
        if (!m->search(&key, (Node *&) stored)) {
            return nullptr;
        } else {
            if (head == stored) {
                return stored->v;
            }
            // 拉回
            if (stored->d) {
                stored->prev = NULL;
                stored->next = head;
                head->prev = stored;
                head = stored;
                size += stored->size;
                stored->d = false;
                gc->remove(stored);
                return stored->v;
            }
            Node *prevNode = stored->prev;
            Node *nextNode = stored->next;
            prevNode->next = nextNode;
            if (nextNode != nullptr)
                nextNode->prev = prevNode;
            stored->next = head;
            stored->prev = nullptr;
            if (head != nullptr)
                head->prev = stored;
            head = stored;
            if (tail == stored) {
                tail = prevNode;
            }
        }
        head->r++;
        return stored->v;
    }
};

#endif /* LRU_hpp */
