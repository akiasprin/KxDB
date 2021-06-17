//
//  AirinCell.cpp
//  KxDB
//
//  Created by 葉朝鈞 on 2021/6/4.
//

#include "AirinCell.hpp"

using namespace std;

AirinCell::AirinCell() {
}

AirinCell::AirinCell(AirinCell *x) {
    if (x->vlen != 0) {
        val = (unsigned char *) new char[x->vlen] { 0 };
        memcpy(val, x->val, x->vlen);
    }
    key = (unsigned char *) new char[x->klen] { 0 };
    memcpy(key, x->key, x->klen);
    ts = time(0);
    klen = x->klen;
    vlen = x->vlen;
}

AirinCell::AirinCell(char *_key, char *_val, int _klen, int _vlen) {
    if (_vlen != 0) {
        val = (unsigned char *) new char[_vlen] { 0 };
        memcpy(val, _val, _vlen);
    }
    key = (unsigned char *) new char[_klen] { 0 };
    memcpy(key, _key, _klen);
    ts = time(0);
    klen = _klen;
    vlen = _vlen;
}

AirinCell::AirinCell(unsigned char *_key, unsigned char *_val, int _klen, int _vlen) {
    if (_vlen != 0) {
        val = (unsigned char *) new char[_vlen] { 0 };
        memcpy(val, _val, _vlen);
    }
    key = (unsigned char *) new char[_klen] { 0 };
    memcpy(key, _key, _klen);
    ts = time(0);
    klen = _klen;
    vlen = _vlen;
}

AirinCell::AirinCell(unsigned char *_key, unsigned char *_val, time_t _ts, int _klen, int _vlen) {
    if (_vlen != 0) {
        val = (unsigned char *) new char[_vlen] { 0 };
        memcpy(val, _val, _vlen);
    }
    key = (unsigned char *) new char[_klen] { 0 };
    memcpy(key, _key, _klen);
    ts  = _ts;
    klen = _klen;
    vlen = _vlen;
}

AirinCell::~AirinCell() {
    if (vlen > 0) {
        delete val;
    }
    delete key;
}

int AirinCell::size() {
    return sizeof ts + sizeof klen + sizeof vlen + klen + vlen;
}


void AirinCell::write(unsigned char *buf)  {
    memcpy(buf, &ts, sizeof(ts));
    memcpy(buf + sizeof(ts), &klen, sizeof(klen));
    memcpy(buf + sizeof(ts) + sizeof(klen), &vlen, sizeof(vlen));
    memcpy(buf + sizeof(ts) + sizeof(klen) + sizeof(vlen), key, klen);
    memcpy(buf + sizeof(ts) + sizeof(klen) + sizeof(vlen) + klen, val, vlen);
}

int AirinCell::read(unsigned char *buf) {
    memcpy(&ts, buf, sizeof(ts));
    memcpy(&klen, buf + sizeof(ts), sizeof(klen));
    memcpy(&vlen, buf + sizeof(ts) + sizeof(klen), sizeof(vlen));
    key = new unsigned char[klen];
    val = new unsigned char[vlen];
    memcpy(key, buf + sizeof(ts) + sizeof(klen) + sizeof(vlen), klen);
    memcpy(val, buf + sizeof(ts) + sizeof(klen) + sizeof(vlen) + klen, vlen);
    return size();
}

bool AirinCell::_lessThan(AirinCell *rhs) {
    if (this == rhs) {
        return false;
    }
    int minKeyLen = min(klen, rhs->klen);
    for (int i = 0; i < minKeyLen; i++) {
        if (key[i] != rhs->key[i])
            return key[i] < rhs->key[i];
    }
    if (klen != rhs->klen) {
        return klen < rhs->klen;
    }
    if (ts != rhs->ts) {
        return ts < rhs->ts;
    }
    return false;
}

bool AirinCell::_equals(AirinCell *rhs) {
    if (this == rhs) {
        return true;
    }
    if (klen != rhs->klen) {
        return false;
    }
    if (ts != rhs->ts) {
        return false;
    }
    for (int i = 0; i < klen; i++) {
        if (key[i] != rhs->key[i]) {
            return false;
        }
    }
    return true;
}

bool AirinCell::lessThan(AirinCell *lhs, AirinCell *rhs) {
    if (lhs == rhs) {
        return false;
    }
    int minKeyLen = min(lhs->klen, rhs->klen);
    for (int i = 0; i < minKeyLen; i++) {
        if (lhs->key[i] != rhs->key[i])
            return lhs->key[i] < rhs->key[i];
    }
    if (lhs->klen != rhs->klen) {
        return lhs->klen < rhs->klen;
    }
    if (lhs->ts != rhs->ts) {
        return lhs->ts < rhs->ts;
    }
    return false;
}

bool AirinCell::equals(AirinCell *lhs, AirinCell *rhs) {
    if (lhs == rhs) {
        return true;
    }
    if (lhs->klen != rhs->klen) {
        return false;
    }
    if (lhs->ts != rhs->ts) {
        return false;
    }
    for (int i = 0; i < lhs->klen; i++) {
        if (lhs->key[i] != rhs->key[i]) {
            return false;
        }
    }
    return true;
}

void AirinCell::debug() {
    cout << "K:";
    for (int i = 0; i < klen; i++)
        cout << key[i];
    cout << ", V:" << vlen << " bytes";
    cout << endl;
}

